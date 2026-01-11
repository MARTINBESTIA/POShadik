//
// Created by marti on 10. 1. 2026.
//
#include "clientThreadsManager.h"
#include "clientSharedInterface.h"

void *inputThreadFunction(void *arg) {
    input_th_data_t *data = arg;

    while (*data->gameStatePtr == 'R') {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms

        int r = select(STDIN_FILENO + 1, &set, NULL, NULL, &tv);
        if (r > 0 && FD_ISSET(STDIN_FILENO, &set)) {
            int ch = getchar();
            if (ch == 'a') changeSnakeDirectionLeft(data);
            else if (ch == 'd') changeSnakeDirectionRight(data);
            else if (ch == 'p') {
                if (*data->gameStatePtr == 'R') *data->gameStatePtr = 'P';
            }
        }
        // r == 0 timeout: loop again and re-check gameStatePtr
    }

    printf("client inputThread ending.\n");
    return NULL;
}

void *outputThreadFunction(void *arg) {
    output_th_data_t *data = (output_th_data_t *)arg;
    while (*data->gameStatePtr == 'R') {
        pthread_mutex_lock(data->updateGameFieldMutexPtr);
        printf("\033[H\033[2J");
        fflush(stdout);
        for (int i = 0; i < data->fieldPtr->fieldLengthY + 2; i++) {
            for (int e = 0 ; e < data->fieldPtr->fieldLengthX + 2; e++) {
                if (i == 0 || i == data->fieldPtr->fieldLengthY + 1 || e == 0 || e == data->fieldPtr->fieldLengthX + 1) {
                    printf("#");
                    continue;
                }
                printf("%c" ,data->fieldPtr->positions[i - 1][e - 1].typeOccupied);
            }
            printf("\n");
        }
        double elapsed = difftime(time(NULL), *data->gameStartTimePtr);
        printf("Time: %.2f\n", elapsed);
        printf("Snake length: %d\n", *data->snakeLengthPtr);
        pthread_mutex_unlock(data->updateGameFieldMutexPtr);
        usleep(100000); // Sleep for 100ms
    }
    printf("client outputThreadEnding.\n");
    return NULL;
}

void *timeClientUpdateThreadFunction(void *arg) {
    time_update_th_data_t *data = (time_update_th_data_t *)arg;
    while (*data->gameStatePtr == 'R') {
        updateLastClientUpdate(data);
        usleep(30000);
    }
    printf("client timeClientThread ending.\n");
    return NULL;
}