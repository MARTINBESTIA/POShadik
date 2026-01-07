#ifndef APP_UTILITIES_H
#define APP_UTILITIES_H
#include <commandsLib/command.h>
#include <commandsLib/syncSharedMemCommandsQueue.h>
#include "appSettings.h"

char * app_utils_make_name_for_element(const char * base, const char * suffix);
typedef struct SharedNames {
    char * sharedMemoryName;
    char * semMutexName;
    char * semProduceName;
    char * semConsumeName;
} shared_names_t;
void shared_names_init(shared_names_t * this, const char * suffix);
void shared_names_destroy(shared_names_t * this);
// Pridané v CV11
// Vymenovaný typ pre módy komunikácie
typedef enum CommunicationMode {
    SHARED_MEMORY, PIPE, SOCKET, COMM_UNKNOWN
} comm_mode_t;
// Štruktúra obsahujúca informácie pre prácu so synchronizovanou zdieľanou pamäťou v aplikácii
typedef struct SharedMemoryCommunication {
    shared_names_t usedNames;
    sync_sh_mem_comm_queue_t queue;
} sh_mem_comm_t;
// Verejné funkcie pre prácu so synchronizovanou zdieľanou pamäťou v aplikácii
void sh_mem_comm_init(sh_mem_comm_t * this, const char * suffix, _Bool withCreation);
void sh_mem_comm_destroy(sh_mem_comm_t * this, _Bool withDeletion);
// Koniec pridaných náležitostí v CV11
#define STATE_STRING_SIZE 101
void * sensor_value_alloc(sensor_mode_t mode);
void * deserialize_value_from_sensor(const command_t * command, sensor_mode_t * mode);
_Bool get_env_value(const char * name, char ** value);
#endif