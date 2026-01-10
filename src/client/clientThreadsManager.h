//
// Created by marti on 10. 1. 2026.
//

#ifndef SEMESTRALKA_CLIENTTHREADSMANAGER_H
#define SEMESTRALKA_CLIENTTHREADSMANAGER_H

#include <pthread.h>
#include "../shared/sharedData.h"

typedef struct InputThreadData {
    pthread_mutex_t* snakeDirectionMutexPtr;
    char* snakeDirectionPtr;
} input_th_data_t;

typedef struct OutputThreadData {
    pthread_mutex_t* updateGameFieldMutexPtr;
    field_t* fieldPtr;
} output_th_data_t;

typedef struct TimeClientUpdateThreadData {
    pthread_mutex_t* clientUpdateMutexPtr;
    time_t* lastClientUpdatePtr;
} time_update_th_data_t;

void *inputThreadFunction(void* arg);
void *outputThreadFunction(void* arg);
void *timeClientUpdateThreadFunction(void* arg);


#endif //SEMESTRALKA_CLIENTTHREADSMANAGER_H