//
// Created by marti on 10. 1. 2026.
//
#include "serverThreadManager.h"
#include "serverSharedInterface.h"

void* updateGameFieldThread(void* data) {
    update_field_th_data_t* threadData = (update_field_th_data_t*)data;
    //while (threadData->isConnectedPtr)
    printf("Starting game field update thread...\n");
    spawnFruit(threadData->fieldPtr);
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(threadData->updateGameFieldMutexPtr);
        int alive = moveSnake(threadData->fieldPtr, threadData->snakePtr, *(threadData->snakeDirectionPtr));
        pthread_mutex_unlock(threadData->updateGameFieldMutexPtr);
        usleep(1000 * 1000); // 1000 ms
    }
    return NULL;
}

void* connectionStatusThread(void* data) {
    connection_status_th_data_t* threadData = (connection_status_th_data_t*)data;
    while (*(threadData->isConnectedPtr)) {
        checkConnectionStatus(threadData->lastClientUpdatePtr, threadData->clientUpdateMutexPtr, threadData->isConnectedPtr);
        usleep(200 * 1000); // 200 ms
    }
    return NULL;
}
