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
            changeSnakeDirectionLeft(data);
        } else if (ch == 'd') {
            changeSnakeDirectionRight(data);
        }
    }
    return NULL;
}

void *outputThreadFunction(void *arg) {
    output_th_data_t *data = (output_th_data_t *)arg;
    //while (1)
    printf("Starting output thread...\n");
    for (int c = 0; c < 1000; c++) {
        pthread_mutex_lock(data->updateGameFieldMutexPtr);
        printf("\033[H\033[2J");
        fflush(stdout);
        for (int i = 0; i < data->fieldPtr->fieldLengthY; i++) {
            for (int e = 0 ; e < data->fieldPtr->fieldLengthX; e++) {
                printf("%c" ,data->fieldPtr->positions[i][e].typeOccupied);
            }
            printf("\n");
        }
        pthread_mutex_unlock(data->updateGameFieldMutexPtr);
        usleep(100000); // Sleep for 100ms
    }
    return NULL;
}

void *timeClientUpdateThreadFunction(void *arg) {
    time_update_th_data_t *data = (time_update_th_data_t *)arg;
    while (1) {
        pthread_mutex_lock(data->clientUpdateMutexPtr);
        updateLastClientUpdate(data);
        pthread_mutex_unlock(data->clientUpdateMutexPtr);
        usleep(30000);
    }
    return NULL;
}