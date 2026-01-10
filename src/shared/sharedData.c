//
// Created by marti on 8. 1. 2026.
//

#include "sharedData.h"
#include <stdlib.h>


void sharedDataInit(shared_data_t* data) {
    pthread_mutex_init(&data->snakeDirectionMutex, NULL);
    pthread_mutex_init(&data->updateGameFieldMutex, NULL);
    data->snakeDirection = DIRECTION_RIGHT;
    data->isConnected = 1;
    data->outputGameField = NULL;
}

void sharedDataDestroy(shared_data_t* data) {
    pthread_mutex_destroy(&data->snakeDirectionMutex);
    pthread_mutex_destroy(&data->updateGameFieldMutex);
    if (data->outputGameField != NULL) {
        free(data->outputGameField);
    }
}


