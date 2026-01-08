//
// Created by marti on 3. 1. 2026.
//

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "../shared/gameConfiguration.h"
#include "../shared/clientServerInterface.h"


int main(void) {

    game_conf_t game_conf;
    shared_data_t sharedData;

    sharedDataInit(&sharedData);
    fillGameConfiguration(&game_conf);

    int shmid = shmget(SHM_KEY_CLIENT_SERVER, sizeof(struct shared_data), 0600|IPC_CREAT|IPC_EXCL);

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
        close(pipe_fd[read_end]);
        if (write(pipe_fd[write_end], &game_conf, sizeof(game_conf_t)) < 0) {;
            perror("write to pipe");
            return 4;
        }
        close(pipe_fd[write_end]);
    }
    usleep(100000);
    return 0;
}