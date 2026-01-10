//
// Created by marti on 10. 1. 2026.
//
#include "clientThreadsManager.h"
#include "clientSharedInterface.h"

void *inputThreadFunction(void *arg) {
    input_th_data_t *data = (input_th_data_t *)arg;
    char ch;
    while (1) {
        ch = getchar();
        if (ch == 'a') {
            pthread_mutex_lock(data->snakeDirectionMutexPtr);
            changeSnakeDirectionLeft(data);
            pthread_mutex_unlock(data->snakeDirectionMutexPtr);
        } else if (ch == 'd') {
            pthread_mutex_lock(data->snakeDirectionMutexPtr);
            changeSnakeDirectionRight(data);
            pthread_mutex_unlock(data->snakeDirectionMutexPtr);
        }
    }
    return NULL;
}

void *outputThreadFunction(void *arg) {
    output_th_data_t *data = (output_th_data_t *)arg;
    while (1) {
        pthread_mutex_lock(data->updateGameFieldMutexPtr);
        // Here you would typically copy the game field data to outputGameFieldPtr
        pthread_mutex_unlock(data->updateGameFieldMutexPtr);
        usleep(50000); // Sleep for 50ms
    }
    return NULL;
}

void *timeClientUpdateThreadFunction(void *arg) {
    time_update_th_data_t *data = (time_update_th_data_t *)arg;
    while (1) {
        pthread_mutex_lock(data->clientUpdateMutexPtr);
        updateLastClientUpdate(data);
        pthread_mutex_unlock(data->clientUpdateMutexPtr);
        usleep(50000); // Update every second
    }
    return NULL;
}