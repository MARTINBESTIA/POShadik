//
// Created by marti on 10. 1. 2026.
//

#ifndef SEMESTRALKA_SERVERTHREADINTERFACE_H
#define SEMESTRALKA_SERVERTHREADINTERFACE_H
#include "gameField.h"
#include "../shared/sharedData.h"
#include <pthread.h>


typedef struct UpdateGameFieldThreadData {
    field_t* fieldPtr;
    snake_position_t* snakePtr;
    pthread_mutex_t* updateGameFieldMutexPtr;
    char* snakeDirectionPtr;
    int* isConnectedPtr;
    char* gameStatePtr;
} update_field_th_data_t;

typedef struct ConnectionStatusThreadData {
    time_t* lastClientUpdatePtr;
    pthread_mutex_t* clientUpdateMutexPtr;
    int* isConnectedPtr;
    char* gameStatePtr;
    time_t* timeDurationPtr;
    time_t* gameStartTimePtr;
} connection_status_th_data_t;

void* updateGameFieldThread(void* arg);
void* connectionStatusThread(void* arg);

#endif //SEMESTRALKA_SERVERTHREADINTERFACE_H