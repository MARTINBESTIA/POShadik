//
// Created by marti on 10. 1. 2026.
//

#include <pthread.h>
#include "clientSharedInterface.h"

void changeSnakeDirectionLeft(input_th_data_t *data) {
    pthread_mutex_lock(data->snakeDirectionMutexPtr);
    if (*data->snakeDirectionPtr == DIRECTION_UP) {
        *data->snakeDirectionPtr  = DIRECTION_LEFT;
    } else if (*data->snakeDirectionPtr  == DIRECTION_DOWN) {
        *data->snakeDirectionPtr  = DIRECTION_RIGHT;
    } else if (*data->snakeDirectionPtr  == DIRECTION_LEFT) {
        *data->snakeDirectionPtr  = DIRECTION_DOWN;
    } else if (*data->snakeDirectionPtr  == DIRECTION_RIGHT) {
        *data->snakeDirectionPtr  = DIRECTION_UP;
    }
    pthread_mutex_unlock(data->snakeDirectionMutexPtr);
}

void changeSnakeDirectionRight(input_th_data_t *data) {
    pthread_mutex_lock(data->snakeDirectionMutexPtr);
    if (*data->snakeDirectionPtr == DIRECTION_UP) {
        *data->snakeDirectionPtr = DIRECTION_RIGHT;
    } else if (*data->snakeDirectionPtr == DIRECTION_DOWN) {
        *data->snakeDirectionPtr = DIRECTION_LEFT;
    } else if (*data->snakeDirectionPtr == DIRECTION_LEFT) {
        *data->snakeDirectionPtr = DIRECTION_UP;
    } else if (*data->snakeDirectionPtr == DIRECTION_RIGHT) {
        *data->snakeDirectionPtr = DIRECTION_DOWN;
    }
    pthread_mutex_unlock(data->snakeDirectionMutexPtr);
}

void readGameField(output_th_data_t *data) {
}


void updateLastClientUpdate(time_update_th_data_t *data) {
    pthread_mutex_lock(data->clientUpdateMutexPtr);
    *data->lastClientUpdatePtr = time(NULL);
    pthread_mutex_unlock(data->clientUpdateMutexPtr);
}