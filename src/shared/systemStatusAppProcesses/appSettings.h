#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#define COMMAND_SEPARATOR "|"
#define COMMAND_END_CHARACTER ";"

#define SHARED_MEMORY_NAME "SHARED_MEMORY"
#define SEM_MUTEX_NAME "SEM_MUTEX"
#define SEM_PRODUCE_NAME "SEM_PRODUCE"
#define SEM_CONSUME_NAME "SEM_CONSUME"
// Pridané v CV11
#define PIPE_NAME "PIPE_NAME"
#define SOCKET_SERVER_NAME "localhost"
#define SOCKET_PORT "5000"
// Koniec pridaných náležitostí v CV11
#define SSA_APP_NAME "systemStatusAppProcesses-app"
#define SSA_SENSOR_NAME "systemStatusAppProcesses-sensor"

#define NO_SESOR_MODES 3
#define NO_PROCESSES_NAME "NO_PROCESSES"
#define NO_PROCESSES_DISABLE_VALUE (-1)
#define HOME_SIZE_NAME "HOME_SIZE"
#define HOME_SIZE_DISABLE_VALUE "-1 ."
#define MAIN_MEM_INFO_NAME "MAIN_MEM_INFO"
#define MAIN_MEM_INFO_DISABLE_VALUE "-1 -1 -1 -1 -1 -1"

typedef enum SensorMode {
    NO_PROCESSES, HOME_SIZE, MAIN_MEM_INFO, S_M_UNKNOWN
} sensor_mode_t;

#endif
