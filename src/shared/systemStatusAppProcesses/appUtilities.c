#include "appUtilities.h"
#include <stdlib.h>
#include <string.h>
#include "appSettings.h"

char * app_utils_make_name_for_element(const char * base, const char * suffix) {
    size_t baseLen = strlen(base);
    char * result = calloc(baseLen + strlen(suffix) + 2, sizeof(char));
    strcpy(result, base);
    result[baseLen] = '_';
    strcpy(result + baseLen + 1, suffix);
    return result;
}

void shared_names_init(shared_names_t * this, const char * suffix) {
    this->sharedMemoryName = app_utils_make_name_for_element(SHARED_MEMORY_NAME, suffix);
    this->semMutexName = app_utils_make_name_for_element(SEM_MUTEX_NAME, suffix);
    this->semProduceName = app_utils_make_name_for_element(SEM_PRODUCE_NAME, suffix);
    this->semConsumeName = app_utils_make_name_for_element(SEM_CONSUME_NAME, suffix);
}

void shared_names_destroy(shared_names_t * this) {
    free(this->sharedMemoryName);
    free(this->semMutexName);
    free(this->semProduceName);
    free(this->semConsumeName);
    memset(this, 0, sizeof(shared_names_t));
}
// Funkcia na inicializáciu synchronizovanej zdieľanej pamäte v aplikácii, pričom je možné ju aj vytvoriť v systéme
void sh_mem_comm_init(sh_mem_comm_t * this, const char * suffix, _Bool withCreation) {
    shared_names_init(&this->usedNames, suffix);
    sync_sh_mem_comm_queue_init(&this->queue, this->usedNames.sharedMemoryName, this->usedNames.semMutexName, this->usedNames.semProduceName, this->usedNames.semConsumeName, withCreation);
    sync_sh_mem_comm_queue_open(&this->queue);
}
// Funkcia na zničenie synchronizovanej zdieľanej pamäte v aplikácii, pričom je možné ju aj zničiť v systéme
void sh_mem_comm_destroy(sh_mem_comm_t * this, _Bool withDeletion) {
    sync_sh_mem_comm_queue_close(&this->queue);
    sync_sh_mem_comm_queue_destroy(&this->queue, withDeletion);
    shared_names_destroy(&this->usedNames);
}
// POZOR! Je potrebné dealokovať!
void * sensor_value_alloc(sensor_mode_t mode) {
    switch (mode) {
        case NO_PROCESSES: {
            return malloc(sizeof(double));
        } case HOME_SIZE: case MAIN_MEM_INFO: {
            return calloc(STATE_STRING_SIZE,sizeof(char));
        } default: {
            return NULL;
        }
    }
}
// POZOR! Je potrebné dealokovať!
void * deserialize_value_from_sensor(const command_t * command, sensor_mode_t * mode) {
    const char * commandString = command_get_command_string(command);
    if (strncmp(commandString, NO_PROCESSES_NAME, strlen(NO_PROCESSES_NAME)) == 0) {
        * mode = NO_PROCESSES;
    } else if (strncmp(commandString, HOME_SIZE_NAME, strlen(HOME_SIZE_NAME)) == 0) {
        * mode = HOME_SIZE;
    } else if (strncmp(commandString, MAIN_MEM_INFO_NAME, strlen(MAIN_MEM_INFO_NAME)) == 0) {
        * mode = MAIN_MEM_INFO;
    } else {
        * mode = S_M_UNKNOWN;
    }
    if (* mode == S_M_UNKNOWN) {
        return NULL;
    }
    void * value = NULL;
    char * dataStart = strchr(commandString, COMMAND_SEPARATOR[0]);
    if (dataStart != NULL) {
        dataStart++;
        value = sensor_value_alloc(* mode);
        switch (* mode) {
            case NO_PROCESSES: {
                double * noProcValue = value;
                * noProcValue = atof(dataStart);
                break;
            } case HOME_SIZE: case MAIN_MEM_INFO: {
                strncpy(value, dataStart, strlen(dataStart) - 1);
                break;
            } default: {
                value = NULL;
            }
        }
    }
    return value;
}

_Bool get_env_value(const char * name, char ** value) {
    * value = getenv(name);
    return * value != NULL;
}
