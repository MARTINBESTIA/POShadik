//
// Created by marti on 8. 1. 2026.
//

#ifndef SEMESTRALKA_CLIENTSERVERINTERFACE_H
#define SEMESTRALKA_CLIENTSERVERINTERFACE_H

#define DIRECTION_UP 'U'
#define DIRECTION_DOWN 'D'
#define DIRECTION_LEFT 'L'
#define DIRECTION_RIGHT 'R'
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef struct shared_data {
    pthread_mutex_t snakeDirectionMutex;
    pthread_mutex_t updateGameFieldMutex;
    char snakeDirection;
    char** outputGameField;
    int isConnected;
} shared_data_t;

void sharedDataInit(shared_data_t* data);
void sharedDataDestroy(shared_data_t* data);

#endif //SEMESTRALKA_CLIENTSERVERINTERFACE_H