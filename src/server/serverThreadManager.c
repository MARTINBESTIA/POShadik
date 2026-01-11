//
// Created by marti on 10. 1. 2026.
//
#include "serverThreadManager.h"
#include "serverSharedInterface.h"

void* updateGameFieldThread(void* data) {
    update_field_th_data_t* threadData = (update_field_th_data_t*)data;
    spawnFruit(threadData->fieldPtr);
    while (*(threadData->isConnectedPtr) && *(threadData->gameStatePtr) != 'E') {
        pthread_mutex_lock(threadData->updateGameFieldMutexPtr);
        int alive = moveSnake(threadData->fieldPtr, threadData->snakePtr, *(threadData->snakeDirectionPtr));
        if (alive == -1) {
            printf("Collision detected! Game Over.\n");
            *(threadData->isConnectedPtr) = 0; // For example, disconnect client
            *(threadData->gameStatePtr) = 'E';
            pthread_mutex_unlock(threadData->updateGameFieldMutexPtr);
            break; // ended
        }
        pthread_mutex_unlock(threadData->updateGameFieldMutexPtr);
        usleep(350 * 1000); // 350 ms
    }
    printf("Server updateGameFieldThread ending\n");
    return NULL;
}

void* connectionStatusThread(void* data) {
    connection_status_th_data_t* threadData = (connection_status_th_data_t*)data;
    while (*(threadData->isConnectedPtr)) {
        checkConnectionStatus(threadData->lastClientUpdatePtr, threadData->clientUpdateMutexPtr, threadData->isConnectedPtr);
        double gameDuration = difftime(time(NULL), *threadData->gameStartTimePtr);
        if (*(threadData->timeDurationPtr) > 0 && gameDuration >= *(threadData->timeDurationPtr)) {
            *(threadData->isConnectedPtr) = 0;
            *(threadData->gameStatePtr) = 'E'; // ended
            break;
        }
        if (*(threadData->gameStatePtr) == 'E') break; // Ended)
        usleep(50 * 1000); // 50 ms
    }
    printf("Server connectionStatusThread ending\n");
    return NULL;
}
