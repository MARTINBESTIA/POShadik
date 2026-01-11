//
// Created by marti on 8. 1. 2026.
//

#include "sharedData.h"
#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>

void sharedDataInit(shared_data_t* data) {

    pthread_mutexattr_t a;
    pthread_mutexattr_init(&a);
    pthread_mutexattr_setpshared(&a, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&data->snakeDirectionMutex, &a);
    pthread_mutex_init(&data->updateGameFieldMutex, &a);
    pthread_mutex_init(&data->clientUpdateMutex, &a);
    pthread_mutex_init(&data->isConnectedMutex, &a);

    pthread_mutexattr_destroy(&a);

    data->field = (field_t){0};
    data->snakeDirection = DIRECTION_RIGHT;
    data->isConnected = 1;
    data->gameState = 'R'; // running
    data->lastClientUpdateTime = time(NULL);
    data->gameStartTime = time(NULL);
    data->snakeLength = 1;
    data->gameFieldPath = 0;
}

void generateGameField(shared_data_t* data) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    }
    char path[256];
    snprintf(path, sizeof(path), "../src/client/gameFieldConfigs/%d", data->gameFieldPath);

    FILE *f = fopen(path, "r");
    if (!f) {
        perror("fopen");
        return;
    }

    int ch;
    int x = 0, y = 0;
    int maxX = 0, maxY = 0;

    while ((ch = fgetc(f)) != EOF) {
        printf("%c\n, ch");
        if (ch == '\n') {
            y++;
            if (x > maxX) {
                maxX = x;
            }
            x = 0;
            continue;
        }
        if (ch == 'X') {
            data->field.positions[y][x].typeOccupied = 'X';
            x++;
        } else {
            data->field.positions[y][x].typeOccupied = ' ';
            x++;
        }
    }
    maxY = y;
    data->field.fieldLengthX = maxX;
    data->field.fieldLengthY = maxY;
    if (ferror(f)) {
        perror("fgetc");
    }
    fclose(f);
}

void sharedDataDestroy(shared_data_t* data) {
    pthread_mutex_destroy(&data->snakeDirectionMutex);
    pthread_mutex_destroy(&data->updateGameFieldMutex);
    pthread_mutex_destroy(&data->clientUpdateMutex);
    pthread_mutex_destroy(&data->isConnectedMutex);
}


