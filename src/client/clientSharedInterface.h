//
// Created by marti on 10. 1. 2026.
//

#ifndef SEMESTRALKA_CLIENTSHAREDINTERFACE_H
#define SEMESTRALKA_CLIENTSHAREDINTERFACE_H
#include "clientThreadsManager.h"
#include "../shared/sharedData.h"


void changeSnakeDirectionLeft(input_th_data_t* data); // client only
void changeSnakeDirectionRight(input_th_data_t* data); // client only
void readGameField(output_th_data_t* data); // client only
void updateLastClientUpdate(time_update_th_data_t* data); // client only


#endif //SEMESTRALKA_CLIENTSHAREDINTERFACE_H