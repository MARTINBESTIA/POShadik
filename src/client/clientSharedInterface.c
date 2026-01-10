//
// Created by marti on 10. 1. 2026.
//

#include <pthread.h>
#include "clientSharedInterface.h"

void changeSnakeDirectionLeft(shared_data_t *data) {
    pthread_mutex_lock(&data->snakeDirectionMutex);
    if (data->snakeDirection == DIRECTION_UP) {
        data->snakeDirection = DIRECTION_LEFT;
    } else if (data->snakeDirection == DIRECTION_DOWN) {
        data->snakeDirection = DIRECTION_RIGHT;
    } else if (data->snakeDirection == DIRECTION_LEFT) {
        data->snakeDirection = DIRECTION_DOWN;
    } else if (data->snakeDirection == DIRECTION_RIGHT) {
        data->snakeDirection = DIRECTION_UP;
    }
    pthread_mutex_unlock(&data->snakeDirectionMutex);
}

void changeSnakeDirectionRight(shared_data_t *data) {
    pthread_mutex_lock(&data->snakeDirectionMutex);
    if (data->snakeDirection == DIRECTION_UP) {
        data->snakeDirection = DIRECTION_RIGHT;
    } else if (data->snakeDirection == DIRECTION_DOWN) {
        data->snakeDirection = DIRECTION_LEFT;
    } else if (data->snakeDirection == DIRECTION_LEFT) {
        data->snakeDirection = DIRECTION_UP;
    } else if (data->snakeDirection == DIRECTION_RIGHT) {
        data->snakeDirection = DIRECTION_DOWN;
    }
    pthread_mutex_unlock(&data->snakeDirectionMutex);
}

void readGameField(shared_data_t *data) {
}


void updateLastClientUpdate(shared_data_t *data) {
    pthread_mutex_lock(&data->clientUpdateMutex);
    data->lastClientUpdate = time(NULL);
    pthread_mutex_unlock(&data->clientUpdateMutex);
}