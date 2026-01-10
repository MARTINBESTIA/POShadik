//
// Created by marti on 4. 1. 2026.
//


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../shared/gameConfiguration.h"
#include "../shared/sharedData.h"
#include "serverThreadManager.h"
#include "gameField.h"
#include "serverSharedInterface.h"


int main(int argc, char** argv) {

    const key_t SHM_KEY_CLIENT_SERVER = 0x12345;

    game_conf_t game_conf;

    read(0, &game_conf, sizeof(game_conf_t));
    close(0);

    int shmid;
    for (;;) {
        shmid = shmget(SHM_KEY_CLIENT_SERVER, sizeof(shared_data_t), 0600); // no CREATE
        if (shmid >= 0) break;
        if (errno == ENOENT) { usleep(10 * 1000); continue; } // wait until created
        perror("server shmget");
        exit(1);
    }
    printf("server shmid %d\n", shmid);
    if (shmid < 0) {
        perror("server:");
        perror("shared memory segment creation failed:");
        return 7;
    }

    void* addr = shmat(shmid, NULL, 0);

    if(addr == NULL)
    {
        perror("Failed to attach shared memory block:");
        return 1;
    }

    shared_data_t* sh_data = (shared_data_t*)addr;

    printf("Game mode: %c\n", game_conf.gameMode);
    printf("Time limit: %d\n", game_conf.timeLimit);
    printf("Game field: %c\n", game_conf.gameField);
    printf("Random generation: %c\n", game_conf.randomGeneration);
    printf("Field length X: %d\n", game_conf.fieldLengthX);
    printf("Field length Y: %d\n", game_conf.fieldLengthY);
    if (game_conf.gameField == 'C') {
        printf("Custom field path: %s\n", game_conf.customFieldPath);
    }

    printf("IsConnected %d\n", sh_data->isConnected);
    printf("Snake direction %c\n", sh_data->snakeDirection);



    snake_position_t snakePosition;
    initializeGameField(&sh_data->field, game_conf.fieldLengthX, game_conf.fieldLengthY, game_conf.randomGeneration);
    initializeSnakePosition(&snakePosition, game_conf.fieldLengthX/2, game_conf.fieldLengthY/2, game_conf.fieldLengthX, game_conf.fieldLengthY);

    update_field_th_data_t updateFieldData = {&sh_data->field, &snakePosition, &sh_data->updateGameFieldMutex, &sh_data->snakeDirection, &sh_data->isConnected};
    connection_status_th_data_t connectionStatusData = {&sh_data->lastClientUpdateTime, &sh_data->clientUpdateMutex, &sh_data->isConnected};


    pthread_t threads[2];
    pthread_create(&threads[0], NULL, &updateGameFieldThread, &updateFieldData);
    //pthread_create(&threads[1], NULL, &connectionStatusThread, &connectionStatusData);


    printf("SERVER BEZII \n");

    pthread_join(threads[0], NULL);
    //pthread_join(threads[1], NULL);


    printf("SERVER KONCI \n");

    destroySnakePosition(&snakePosition);
    sharedDataDestroy(sh_data);

    if(shmdt(addr) != 0)
    {
        perror("Failed to detach shared memory block:");
        return 1;
    }

    if(shmctl(shmid, IPC_RMID, NULL) != 0)
    {
        perror("Failed to remove shared memory block:");
        return 1;
    }
    return 0;
}
