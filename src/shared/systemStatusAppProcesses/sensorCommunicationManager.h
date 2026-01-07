#ifndef SENSOR_COMMUNICATION_MANAGER_H
#define SENSOR_COMMUNICATION_MANAGER_H
#include <stdatomic.h>
#include "appUtilities.h"
#include <pthread.h>
#include <commandsLib/commandsQueue.h>
// Štruktúra obsahujúca všetko potrebné pre komunikátor
typedef struct SensorCommunicationManager {
    // Senzor <--> Komunikátor
    comm_mode_t mode;
    void * communicationData;
    pid_t sensorProcess;
    // Manažér <--> Komunikátor
    commands_queue_t * queue;
    pthread_mutex_t * mutex;
    pthread_cond_t * produce;
    pthread_cond_t * consume;
    atomic_bool isRunning;
} scm_data_t;
// Hlavičky funkcií, ktoré sú verejne dostupné a pracujú s komunikátorom
pthread_t scm_start(scm_data_t * this, comm_mode_t mode, int delay, const char * commData, const char * basePath, commands_queue_t * queue, pthread_mutex_t * mutex, pthread_cond_t * produce, pthread_cond_t * consume);
void scm_stop(scm_data_t * this);
void scm_destroy(scm_data_t * this);
#endif
