#include "sensorCommunicationManager.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pipeSocketLib/pipe.h>
#include <pipeSocketLib/socket.h>
// Funkcia na inicializáciu komunikátora, pričom sa určuje typ komunikácie, údaje komunikácie a všetko potrebné pre komunikáciu s manažérom
static void scm_init(scm_data_t * this, comm_mode_t commMode, int delay, const char * commData, const char * basePath, commands_queue_t * queue, pthread_mutex_t * mutex, pthread_cond_t * produce, pthread_cond_t * consume) {
    this->mode = commMode;
    sensor_mode_t sensorMode = S_M_UNKNOWN;
    switch (this->mode) {
        case SHARED_MEMORY: {
            // Informácia o počte procesov bude používať synchronizovanú zdieľanú pamäť
            sh_mem_comm_t * data = calloc(1, sizeof(sh_mem_comm_t));
            this->communicationData = data;
            sh_mem_comm_init(data, commData, 1);
            sensorMode = NO_PROCESSES;
            break;
        } case PIPE: {
            // Informácia o veľkosti domovského adresára bude používať pomenovaný dátovod
            sensorMode = HOME_SIZE;
            break;
        } case SOCKET: {
            // Informácia o opečnej pamäti bude používať sokety
            sensorMode = MAIN_MEM_INFO;
            break;
        } default: {
            fprintf(stderr, "scm_init: zle zadany mod komunikacie -- %d!\n", this->mode);
            exit(EXIT_FAILURE);
        }
    }
    pid_t result = fork();
    if (result < 0) {
        fprintf(stderr, "scm_init: fork zlyhal!\n");
        switch (this->mode) {
            case SHARED_MEMORY: {
                // Je potrebné zničiť zdieľanú pamäť, keďže bola vytvorená ešte pred fork-om
                sh_mem_comm_t * data = this->communicationData;
                sh_mem_comm_destroy(data, 1);
                break;
            } default: {
                // Nie je potrebné nič robiť
                break;
            }
        }
        exit(EXIT_FAILURE);
    }
    if (result == 0) {
        switch (this->mode) {
            case SHARED_MEMORY: {
                // Je potrebné zničiť zdieľanú pamäť bez jej vymazania zo systému, keďže potomok použije exec
                sh_mem_comm_t * data = this->communicationData;
                sh_mem_comm_destroy(data, 0);
                break;
            } default: {
                // Nie je potrebné nič robiť
                break;
            }
        }
        size_t baseLen = strlen(basePath);
        size_t sensorLen = strlen(SSA_SENSOR_NAME);
        char path[ baseLen + sensorLen + 2];
        strncpy(path, basePath, baseLen);
        path[baseLen] = '/';
        strncpy(path + baseLen + 1, SSA_SENSOR_NAME, sensorLen);
        path[baseLen + sensorLen + 1] = '\0';
#define TMP_INT_CHAR_SIZE 11
        char delayChar[TMP_INT_CHAR_SIZE] = {0};
        sprintf(delayChar, "%d", delay);
        char sensorModeChar[TMP_INT_CHAR_SIZE] = {0};
        sprintf(sensorModeChar, "%d", sensorMode);
        // Pridaný komunikačný režim
        char commModeChar[TMP_INT_CHAR_SIZE] = {0};
        sprintf(commModeChar, "%d", commMode);
#undef TMP_INT_CHAR_SIZE
        execlp(path, path, sensorModeChar, commModeChar, commData, delayChar, NULL);
    }
    switch (this->mode) {
        case PIPE: {
            // Príprava pomenovaného dátovodu, pričom sa pri názve používajú podobné pravidlá ako pri zdieľanej pamäti
            pipe_data_t * data = calloc(1, sizeof(pipe_data_t));
            this->communicationData = data;
            char * pipeName = app_utils_make_name_for_element(PIPE_NAME, commData);
            // Komunikátor zodpovedá za vytvorenie dátovodu
            pipe_init(data, pipeName, 1);
            // Komunikátor bude údaje čítať a následne ich pošle manažérovi
            pipe_open_read(data);
            break;
        } case SOCKET: {
            // Príprava serverovej časti komunikácie pomocou soketov
            socket_server_t * data = calloc(1, sizeof(socket_server_t));
            this->communicationData = data;
            socket_server_init(data, atoi(commData));
            printf("Komunikator -- soket: čakám na klienta!\n");
            socket_server_accept_connection(data);
            break;
        } default: {
            // Nie je potrebné nič robiť
            break;
        }
    }
    this->sensorProcess = result;
    // Manažér
    this->queue = queue;
    this->mutex = mutex;
    this->consume = consume;
    this->produce = produce;
    this->isRunning = (atomic_bool) 1;
}
// Funkcia vlákna komunikátora
static void * scm_do_management(void * arg) {
    scm_data_t * this = arg;
    command_t command;
    while (this->isRunning) {
        // Výber hodnoty podľa módu komunikátora
        memset(&command, 0, sizeof(command_t));
        switch (this->mode) {
            case SHARED_MEMORY: {
                sh_mem_comm_t * data = this->communicationData;
                sync_sh_mem_comm_queue_dequeue(&data->queue, &command);
                break;
            } case PIPE: {
                pipe_data_t * data = this->communicationData;
                char tmpCommand[COMMAND_MAX_CAPACITY];
                pipe_read(data, tmpCommand, COMMAND_MAX_CAPACITY);
                command_init(&command, tmpCommand, COMMAND_SEPARATOR, COMMAND_END_CHARACTER);
                break;
            } case SOCKET: {
                socket_server_t * data = this->communicationData;
                char tmpCommand[COMMAND_MAX_CAPACITY];
                socket_read(&data->activeSocket, tmpCommand, COMMAND_MAX_CAPACITY);
                command_init(&command, tmpCommand, COMMAND_SEPARATOR, COMMAND_END_CHARACTER);
                break;
            } default: {
                fprintf(stderr, "scm_do_management: zly mod komunikacie -- %d!\n", this->mode);
                exit(EXIT_FAILURE);
            }
        }
        // Vloženie do spoločného frontu príkazov, z ktorým pracujú ostatné komunikátory a manažér
        pthread_mutex_lock(this->mutex);
        while (commands_queue_is_full(this->queue) && this->isRunning) {
            pthread_cond_wait(this->produce, this->mutex);
        }
        if (this->isRunning) {
            commands_queue_enqueue(this->queue, &command);
            pthread_cond_signal(this->consume);
        }
        pthread_mutex_unlock(this->mutex);
    }
    pthread_exit(NULL);
}
// Funkcia na spustenie komunikátora, pričom potrebuje aj mód, údaje komunikácie oneskorenie, základnú cestu a náležitosti pre komunikáciu s manažérom
pthread_t scm_start(scm_data_t * this, comm_mode_t mode, int delay, const char * commData, const char * basePath, commands_queue_t * queue, pthread_mutex_t * mutex, pthread_cond_t * produce, pthread_cond_t * consume) {
    scm_init(this, mode, delay, commData, basePath, queue, mutex, produce, consume);
    pthread_t commManThread;
    pthread_create(&commManThread, NULL, scm_do_management, this);
    return commManThread;
}
// Funkcia na zastavenie komunikátora
void scm_stop(scm_data_t * this) {
    this->isRunning = (atomic_bool) 0;
    // Vzhľadom na to, že komunikátorov môže byť potenciálne viac, je potrebné použiť broadcast
    pthread_cond_broadcast(this->produce);
}
// Funkcia na zničenie komunikátora
void scm_destroy(scm_data_t * this) {
    // Ak komunikátor beží, nedá sa zničiť!
    if (this->isRunning) {
        fprintf(stderr, "scm_destroy: komunikator stale bezi!\n");
        return;
    }
    // Zastavenie senzora
#define TMP_INT_CHAR_SIZE 11
    char pidValue[TMP_INT_CHAR_SIZE] = {0};
    sprintf(pidValue, "%d", this->sensorProcess);
    char commandSignal[31] = {0};
    char * commandKill = "kill -TERM ";
    strcpy(commandSignal, commandKill);
    strcpy(commandSignal + strlen(commandKill), pidValue);
#undef TMP_INT_CHAR_SIZE
    system(commandSignal);
    int status;
    waitpid(this->sensorProcess, &status, 0);
    // Vyčistenie komunikačnej časti so senzorom
    switch (this->mode) {
        case SHARED_MEMORY: {
            sh_mem_comm_t * data = this->communicationData;
            sh_mem_comm_destroy(data, 1);
            memset(data, 0, sizeof(sh_mem_comm_t));
            break;
        } case PIPE: {
            pipe_data_t * data = this->communicationData;
            pipe_close(data);
            pipe_destroy(data, 1, 1);
            memset(data, 0, sizeof(pipe_data_t));
            break;
        } case SOCKET: {
            socket_server_t * data = this->communicationData;
            socket_server_destroy(data);
            memset(data, 0, sizeof(socket_server_t));
            break;
        } default: {
            fprintf(stderr, "scm_destroy: zly mod komunikacie -- %d!\n", this->mode);
            exit(EXIT_FAILURE);
        }
    }
    this->mode = COMM_UNKNOWN;
    this->sensorProcess = 0;
    free(this->communicationData);
    this->communicationData = NULL;
    // Vynulovanie zvyšných položiek -- za ne zodpovedá manažér
    this->queue = NULL;
    this->mutex = NULL;
    this->produce = NULL;
    this->consume = NULL;
}