//
// Created by marti on 3. 1. 2026.
//

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "../shared/gameConfiguration.h"
#include "../shared/gameFieldGenerator.h"


int main(void) {

    game_conf_t game_conf;

    fillGameConfiguration(&game_conf);

    int pipe_fd[2];

    const int write_end = 0;
    const int read_end = 1;

    pid_t pid = fork();
    printf("Forknuté PID: %d\n", pid);
    if(pipe(pipe_fd) < 0)
    {
        perror("Error creating pipe");
        return 1;
    }

    write(pipe_fd[write_end], &game_conf, sizeof(game_conf_t));

    if(pid < 0)
    {
        perror("fork");
        return 2;
    }
    if(pid > 0)
    {
        execl("./server", "./server", NULL);
        perror("execl");
        return 3;
    }




    return 0;
}