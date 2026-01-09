//
// Created by marti on 9. 1. 2026.
//

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "../shared/gameConfiguration.h"
#include "../shared/clientServerInterface.h"
#include <pthread.h>

#include "gameInitializer.h"

int initializeGame() {

    game_conf_t game_conf;
    shared_data_t sharedData;

    const key_t SHM_KEY_CLIENT_SERVER = 0x12345;
    fillGameConfiguration(&game_conf);
    int pipe_fd[2];

    const int write_end = 1;
    const int read_end = 0;

    if(pipe(pipe_fd) < 0)
    {
        perror("Error creating pipe");
        return 1;
    }

    pid_t pid = fork();
    printf("Forknuté PID: %d\n", pid);

    if(pid < 0)
    {
        perror("fork");
        return 2;
    }

    if (pid == 0)
    {
        //server process
        if (setsid() < 0) {
            perror("setsid");
            _exit(1);
        }

        close(pipe_fd[write_end]);
        if (dup2(pipe_fd[read_end], 0)  < 0) {
            perror("dup2");
            return 5;
        }
        close(pipe_fd[read_end]);
        if (execl("./server", "server", (char*)NULL) < 0) {
            perror("execl");
            return 6;
        }
    }

    if(pid > 0)
    {
        //client process

        int shmid = shmget(SHM_KEY_CLIENT_SERVER, sizeof(shared_data_t), 0600|IPC_CREAT|IPC_EXCL);
        printf("%d\n", shmid);

        if (shmid < 0) {
            perror("client process:");
            perror("shared memory segment creation failed:");
            return 7;
        }

        void* addr = shmat(shmid, NULL, 0);

        if(addr == NULL)
        {
            perror("Failed to attach shared memory block:");
            return 1;
        }

        shared_data_t* data = (shared_data_t*)addr;
        sharedDataInit(data);

        close(pipe_fd[read_end]);
        if (write(pipe_fd[write_end], &game_conf, sizeof(game_conf_t)) < 0) {
            perror("write to pipe");
            return 4;
        }
        close(pipe_fd[write_end]);
    }
    return 0;
}