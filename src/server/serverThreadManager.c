//
// Created by marti on 10. 1. 2026.
//
#include "serverThreadManager.h"
#include "serverSharedInterface.h"

void* updateGameFieldThread(void* data) {
    update_field_th_data_t* threadData = (update_field_th_data_t*)data;
    while (threadData->isConnectedPtr) {
        pthread_mutex_lock(threadData->updateGameFieldMutexPtr);
        int alive = moveSnake(threadData->fieldPtr, threadData->snakePtr, *(threadData->snakeDirectionPtr));
        pthread_mutex_unlock(threadData->updateGameFieldMutexPtr);
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
