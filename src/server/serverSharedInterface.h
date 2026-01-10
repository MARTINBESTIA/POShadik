//
// Created by marti on 10. 1. 2026.
//

#ifndef SEMESTRALKA_SERVERSHAREDINTERFACE_H
#define SEMESTRALKA_SERVERSHAREDINTERFACE_H

#include "../shared/sharedData.h"

char readSnakeDirection(shared_data_t* data, char* direction);
void updateGameField(shared_data_t* data);
void checkConnectionStatus(shared_data_t* data);

#endif //SEMESTRALKA_SERVERSHAREDINTERFACE_H