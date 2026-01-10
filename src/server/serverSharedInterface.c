//
// Created by marti on 10. 1. 2026.
//

#include <pthread.h>
#include "serverSharedInterface.h"


char readSnakeDirection(shared_data_t *data, char *direction) {
    pthread_mutex_lock(&data->snakeDirectionMutex);
    *direction = data->snakeDirection;
    pthread_mutex_unlock(&data->snakeDirectionMutex);
    return *direction;
}

void updateGameField(shared_data_t *data) {
}

void checkConnectionStatus(shared_data_t *data) {
    pthread_mutex_lock(&data->clientUpdateMutex);
    time_t currentTime = time(NULL);
    double diff = difftime(currentTime, data->lastClientUpdate);
    if (diff > 5.0) {
        data->isConnected = 0;
    } else {
        data->isConnected = 1;
    }
    pthread_mutex_unlock(&data->clientUpdateMutex);
}