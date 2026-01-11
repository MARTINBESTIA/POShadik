//
// Created by marti on 8. 1. 2026.
//

#include "sharedData.h"
#include <stdlib.h>


void sharedDataInit(shared_data_t* data) {

    pthread_mutexattr_t a;
    pthread_mutexattr_init(&a);
    pthread_mutexattr_setpshared(&a, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&data->snakeDirectionMutex, &a);
    pthread_mutex_init(&data->updateGameFieldMutex, &a);
    pthread_mutex_init(&data->clientUpdateMutex, &a);
    pthread_mutex_init(&data->isConnectedMutex, &a);

    pthread_mutexattr_destroy(&a);

    data->field = (field_t){0};
    data->snakeDirection = DIRECTION_RIGHT;
    data->isConnected = 1;
    data->gameState = 'R'; // running
    data->lastClientUpdateTime = time(NULL);
    data->gameStartTime = time(NULL);
    data->snakeLength = 1;
}

void sharedDataDestroy(shared_data_t* data) {
    pthread_mutex_destroy(&data->snakeDirectionMutex);
    pthread_mutex_destroy(&data->updateGameFieldMutex);
    pthread_mutex_destroy(&data->clientUpdateMutex);
    pthread_mutex_destroy(&data->isConnectedMutex);
}


