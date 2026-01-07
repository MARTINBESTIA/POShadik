#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pipeSocketLib/pipe.h>
#include <pipeSocketLib/socket.h>
#include "appSettings.h"
#include "appUtilities.h"
// Štruktúra obsahujúca potrebné náležitosti pre senzor a jeho komunikáciu
typedef struct {
    comm_mode_t communicationMode;
    void * communicationData;
    sensor_mode_t sensorMode;
    int delay;
} sensor_data_t;

static volatile atomic_bool isRunning;

void apply_term_signal(int signal) {
    if (signal == SIGTERM) {
        isRunning = (atomic_bool) 0;
    }
}
// Funkcia na inicializáciu senzora, pričom je pridaný aj mód komunikácie
static void sensor_init(sensor_data_t * this, comm_mode_t commMode, char * commData, sensor_mode_t mode, int delay) {
    switch(commMode) {
        case SHARED_MEMORY: {
            this->communicationData = calloc(1, sizeof(sh_mem_comm_t));
            sh_mem_comm_init(this->communicationData, commData, 0);
            break;
        } case PIPE: {
            // Vytvorenie pomenovaného dátovodu, pričom sa predpokladá už jeho exstencia v systéme
            this->communicationData = calloc(1, sizeof(pipe_data_t));
            pipe_init(this->communicationData, app_utils_make_name_for_element(PIPE_NAME, commData), 0);
            pipe_open_write(this->communicationData);
            break;
        } case SOCKET: {
            // Vytvorenie soketu pre klienta, pričom sa čaká 1 sekundu, aby sa server stihol pripraviť na akceptáciu pripojení
            this->communicationData = calloc(1, sizeof(socket_client_t));
            sleep(1);
            socket_client_init(this->communicationData, SOCKET_SERVER_NAME, commData);
            break;
        } default: {
            fprintf(stderr, "sensor_init: neplatny mod komunikacie -- %d!\n", commMode);
            exit(EXIT_FAILURE);
        }
    }
    this->communicationMode = commMode;
    this->sensorMode = mode;
    this->delay = delay;
    isRunning = (atomic_bool) 1;
}
// Funkcia na zničenie senzora, pričom sa pridal aj mód komunikácie
static void sensor_destroy(sensor_data_t * this) {
    // Ak senzor beží, nie je možné ho zničiť!
    if (isRunning) {
        fprintf(stderr, "sensor_destroy: senzor este stale bezi! Neda sa znicit!\n");
        return;
    }
    // Zničenie, resp. zavretie komunikačných prostriedkov
    switch (this->communicationMode) {
        case SHARED_MEMORY: {
            sh_mem_comm_destroy(this->communicationData, 0);
            break;
        } case PIPE: {
            pipe_close(this->communicationData);
            pipe_destroy(this->communicationData, 0, 1);
            break;
        } case SOCKET: {
            socket_client_destroy(this->communicationData);
            break;
        } default: {
            fprintf(stderr, "sensor_destroy: zly mod komunikacie -- %d!\n", this->communicationMode);
            exit(EXIT_FAILURE);
        }
    }
    free(this->communicationData);
    this->communicationData = NULL;
    this->communicationMode = COMM_UNKNOWN;
    this->sensorMode = S_M_UNKNOWN;
    this->delay = 0;
    isRunning = (atomic_bool) 0;
}
// Funkcia na získanie hodnoty zo súboru podľa módu senzora, pričom sa hodnota nachádza v príslušnom súbore, ktorý sa po získaní hodnoty vymaže
static _Bool sensor_value_try_get_from_file(const char * fileName, sensor_mode_t mode, void * value) {
    FILE * logFile = fopen(fileName, "r");
    if (logFile != NULL) {
        switch (mode) {
            case NO_PROCESSES: {
                fscanf(logFile, "%lf", (double *) value);
                break;
            } case HOME_SIZE: case MAIN_MEM_INFO: {
                fgets(value, STATE_STRING_SIZE, logFile);
                char * newLine = strchr(value, '\n');
                if (newLine != NULL) {
                    *newLine = '\0';
                }
                break;
            } default: {
                fprintf(stderr, "sensor_value_try_get_from_file: senzor nema spravne zadany mod!\n");
                break;
            }
        }
        fclose(logFile);
        remove(fileName);
        return 1;
    }
    return 0;
}

static _Bool sensor_value_prepare(sensor_mode_t mode, void * value) {
    char * fileName = "";
    switch (mode) {
        case NO_PROCESSES: {
            double * noProc = value;
            * noProc = NO_PROCESSES_DISABLE_VALUE;
            system("ps --no-header -u $USER | head -n-3 | wc -l > result_no_proc.log");
            fileName = "result_no_proc.log";
            break;
        } case HOME_SIZE: {
            strncpy(value, HOME_SIZE_DISABLE_VALUE, strlen(HOME_SIZE_DISABLE_VALUE) + 1);
            system("du -sh $HOME > result_home_size.log 2> /dev/null");
            fileName = "result_home_size.log";
            break;
        } case MAIN_MEM_INFO: {
            strncpy(value, MAIN_MEM_INFO_DISABLE_VALUE, strlen(MAIN_MEM_INFO_DISABLE_VALUE) + 1);
            system("free -ht | head -2 | tail -1 | tr -s [:blank:] | cut -d: -f2 | cut -d\\  -f2-7 > result_main_mem_info.log");
            fileName = "result_main_mem_info.log";
            break;
        } default: {
            fprintf(stderr, "sensor_value_prepare: senzor nema spravne zadany mod!\n");
        }
    }
    return sensor_value_try_get_from_file(fileName, mode, value);
}

int main(int argc, char * argv[]) {
    if (argc < 5) {
        fprintf(stderr, "main: senzor vyzaduje tieto argumenty: %s mod mod_komunikacie komunikacny_udaj pocet_sekund!\n", argv[0]);
        return EXIT_FAILURE;
    }
    signal(SIGTERM, apply_term_signal);
    sensor_data_t sensorData;
    int delay = atoi(argv[4]);
    sensor_mode_t mode = atoi(argv[1]);
    comm_mode_t commMode = atoi(argv[2]);
    printf("Senzor mod %d s komunkaciou typu %d vola init!\n", mode, commMode);
    sensor_init(&sensorData, commMode, argv[3], mode, delay == 0 ? 1 : delay);
    void * value = sensor_value_alloc(sensorData.sensorMode);
    printf("Senzor mod %d s komunkaciou typu %d ide do slucky!\n", mode, commMode);
    while (isRunning) {
        if (sensor_value_prepare(sensorData.sensorMode, value)) {
            command_t command;
            command_init(&command,"",COMMAND_SEPARATOR, COMMAND_END_CHARACTER);
            switch (sensorData.sensorMode) {
                case NO_PROCESSES: {
                    command_try_append_string(&command,NO_PROCESSES_NAME, 0);
                    char representation[11];
                    double noProc = * ((double * ) value);
                    sprintf(representation,"%.0lf", noProc);
                    command_try_append_string(&command,representation, 1);
                    break;
                } case HOME_SIZE: {
                    command_try_append_string(&command,HOME_SIZE_NAME, 0);
                    command_try_append_string(&command, value, 1);
                    break;
                } case MAIN_MEM_INFO: {
                    command_try_append_string(&command,MAIN_MEM_INFO_NAME, 0);
                    command_try_append_string(&command, value, 1);
                    break;
                } default: {
                    fprintf(stderr, "main: senzor nema spravne zadany mod -- %d!\n", sensorData.sensorMode);
                    continue;
                }
            }
            // Pridané poslanie príslušnej informácie podľa módu komunikácie
            switch (sensorData.communicationMode) {
                case SHARED_MEMORY: {
                    sh_mem_comm_t * data = sensorData.communicationData;
                    sync_sh_mem_comm_queue_enqueue(&data->queue, &command);
                    break;
                } case PIPE: {
                    pipe_write(sensorData.communicationData, command_get_command_string(&command), COMMAND_MAX_CAPACITY);
                    break;
                } case SOCKET: {
                    socket_write(sensorData.communicationData, command_get_command_string(&command), COMMAND_MAX_CAPACITY);
                    break;
                } default: {
                    fprintf(stderr, "main: senzor nema spravne zadany mod komunikacie -- %d!\n", sensorData.communicationMode);
                    break;
                }
            }
        }
        sleep(sensorData.delay);
    }
    printf("Senzor mod %d s komunkaciou typu %d vychaza do slucky!\n", mode, commMode);
    free(value);
    sensor_destroy(&sensorData);
    printf("Senzor mod %d s komunkaciou typu %d konci!\n", mode, commMode);
    return 0;
}