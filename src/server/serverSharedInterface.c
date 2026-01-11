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

int checkConnectionStatus(time_t* lastClientUpdatePtr, pthread_mutex_t* clientUpdateMutexPtr, int* isConnectedPtr) {
    pthread_mutex_lock(clientUpdateMutexPtr);
    time_t currentTime = time(NULL);
    int result = 0;
    double diffClientConnected = difftime(currentTime, *lastClientUpdatePtr);
    if (diffClientConnected > 60.0) {
        result = -1;
    } else
    if (diffClientConnected > 1.0) {
        *isConnectedPtr = 0;
        result = 1;
    } else {
        *isConnectedPtr = 1;
        result = 0;
    }
    pthread_mutex_unlock(clientUpdateMutexPtr);

}