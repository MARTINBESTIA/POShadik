//
// Created by marti on 10. 1. 2026.
//

#ifndef SEMESTRALKA_SERVERSHAREDINTERFACE_H
#define SEMESTRALKA_SERVERSHAREDINTERFACE_H

#include "../shared/sharedData.h"

char readSnakeDirection(shared_data_t* data, char* direction);
int checkConnectionStatus(time_t* lastClientUpdatePtr, pthread_mutex_t* clientUpdateMutexPtr, int* isConnectedPtr);

#endif //SEMESTRALKA_SERVERSHAREDINTERFACE_H