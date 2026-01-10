//
// Created by marti on 10. 1. 2026.
//

#ifndef SEMESTRALKA_CLIENTSHAREDINTERFACE_H
#define SEMESTRALKA_CLIENTSHAREDINTERFACE_H
#include "../shared/sharedData.h"


void changeSnakeDirectionLeft(shared_data_t* data); // client only
void changeSnakeDirectionRight(shared_data_t* data); // client only
void readGameField(shared_data_t* data); // client only
void updateLastClientUpdate(shared_data_t* data); // client only


#endif //SEMESTRALKA_CLIENTSHAREDINTERFACE_H