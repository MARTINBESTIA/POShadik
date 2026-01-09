//
// Created by marti on 3. 1. 2026.
//

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "../shared/gameConfiguration.h"
#include "../shared/clientServerInterface.h"
#include "gameInitializer.h"


int main(void) {

    const key_t SHM_KEY_CLIENT_SERVER = 0x12345;
    int serverExists = 0;
    int shmid = shmget(SHM_KEY_CLIENT_SERVER, sizeof(shared_data_t), 0600|IPC_CREAT|IPC_EXCL);
    if (shmid < 0) {
        if (errno == EEXIST) {
            // segment already exists -> server already running
            serverExists = 1;
            printf("server exists\n");
        } else {
            perror("shmget");
            return 1;
        }
    } else {
        shmctl(shmid, IPC_RMID, NULL);
    }

    if (!serverExists) {
        if (initializeGame() != 0) {
            fprintf(stderr, "main: inicializacia hry zlyhala!\n");
            return 1;
        }
    } else {
        fprintf(stderr, "main: server uz bezi!\n");
    }

    printf("klient bezi\n");
    sleep(10);
    printf("klient dobezal\n");

    return 0;
}