//
// Created by marti on 10. 1. 2026.
//

#include <pthread.h>
#include "serverSharedInterface.h"

#include "serverThreadManager.h"


char readSnakeDirection(shared_data_t *data, char *direction) {
    pthread_mutex_lock(&data->snakeDirectionMutex);
    *direction = data->snakeDirection;
    pthread_mutex_unlock(&data->snakeDirectionMutex);
    return *direction;
}

void checkConnectionStatus(time_t* lastClientUpdatePtr, pthread_mutex_t* clientUpdateMutexPtr, int* isConnectedPtr) {
    pthread_mutex_lock(clientUpdateMutexPtr);
    time_t currentTime = time(NULL);
    double diffClientConnected = difftime(currentTime, *lastClientUpdatePtr);
    if (diffClientConnected > 5.0) {
        *isConnectedPtr = 0;
    } else {
        *isConnectedPtr = 1;
    }
    pthread_mutex_unlock(clientUpdateMutexPtr);

}