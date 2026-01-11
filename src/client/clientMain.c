//
// Created by marti on 3. 1. 2026.
//

#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "../shared/gameConfiguration.h"
#include "../shared/sharedData.h"
#include "../shared/gameConfiguration.h"
#include "../shared/sharedData.h"
#include "clientThreadsManager.h"
#include <pthread.h>
#include "gameInitializer.h"

static struct termios oldt;

static void disable_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

static void enable_raw_mode(void) {
    struct termios newt;
    if (tcgetattr(STDIN_FILENO, &oldt) == -1) return;
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // no line buffering, no echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    atexit(disable_raw_mode);
} // enable raw mode on exit, AI generated


int main(void) {

    const key_t SHM_KEY_CLIENT_SERVER = 0x12345;

    int serverExists = 0;
    int shmid = shmget(SHM_KEY_CLIENT_SERVER, sizeof(shared_data_t), 0600 | IPC_CREAT | IPC_EXCL);
    if (shmid < 0) {
        if (errno == EEXIST) {
            serverExists = 1;
            printf("server exists\n");
        } else {
            perror("shmget");
            return 1;
        }
    } else {
        shmctl(shmid, IPC_RMID, NULL);
    }

    while (1) {
        printf("Hey, what do you want to do?:\n"
               "N: Play, new game\n");
        if (serverExists) printf("C: Continue playing existing game\n");
        printf("E: End program\n");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'E') {
            printf("Exiting program.\n");
            return 0;
        } else if (choice == 'N') {
            if (serverExists) {
                printf("Starting a new game will terminate the existing game. Are you sure? (Y/N): ");
                char confirm;
                scanf(" %c", &confirm);
                if (confirm != 'Y') {
                    continue;
                }
                // delete existing shared memory
                int shmid = shmget(SHM_KEY_CLIENT_SERVER, sizeof(shared_data_t), 0600);
                if (shmid >= 0) {
                    shmctl(shmid, IPC_RMID, NULL);
                }
            }
            serverExists = 0;
            // start new game
            game_conf_t game_conf;
            shared_data_t sharedData;

            fillGameConfiguration(&game_conf);
            int pipe_fd[2];

            const int write_end = 1;
            const int read_end = 0;

            if (pipe(pipe_fd) < 0) {
                perror("Error creating pipe");
                return 1;
            }

            pid_t pid = fork();
            printf("Forknuté PID: %d\n", pid);

            if (pid < 0) {
                perror("fork");
                return 2;
            }

            if (pid == 0) {
                //server process
                if (setsid() < 0) {
                    perror("setsid");
                    _exit(1);
                }

                close(pipe_fd[write_end]);
                if (dup2(pipe_fd[read_end], 0) < 0) {
                    perror("dup2");
                    return 5;
                }
                close(pipe_fd[read_end]);
                if (execl("./server", "server", (char *) NULL) < 0) {
                    perror("execl");
                    return 6;
                }
            }

            if (pid > 0) {
                //client process
                enable_raw_mode();
                int shmid = shmget(SHM_KEY_CLIENT_SERVER, sizeof(shared_data_t), 0600 | IPC_CREAT | IPC_EXCL);
                printf("%d\n", shmid);

                if (shmid < 0) {
                    perror("client process:");
                    perror("shared memory segment creation failed:");
                    return 7;
                }

                void *addr = shmat(shmid, NULL, 0);

                if (addr == NULL) {
                    perror("Failed to attach shared memory block:");
                    return 1;
                }

                shared_data_t *sh_data = (shared_data_t *) addr;
                sh_data->gameDuration = (game_conf.gameMode == 'T') ? game_conf.timeLimit : -1;
                sharedDataInit(sh_data);

                close(pipe_fd[read_end]);
                if (write(pipe_fd[write_end], &game_conf, sizeof(game_conf_t)) < 0) {
                    perror("write to pipe");
                    return 4;
                }
                close(pipe_fd[write_end]);

                pthread_t threads[3];

                input_th_data_t inputThreadData = {
                    &sh_data->snakeDirectionMutex, &sh_data->snakeDirection, &sh_data->gameState
                };
                output_th_data_t outputThreadData = {
                    &sh_data->updateGameFieldMutex, &sh_data->field, &sh_data->gameState, &sh_data->gameStartTime, &sh_data->snakeLength
                };
                time_update_th_data_t updateTimeThreadData = {
                    &sh_data->clientUpdateMutex, &sh_data->lastClientUpdateTime, &sh_data->gameState
                };

                printf("klient bezi\n");

                pthread_create(&threads[0], NULL, &timeClientUpdateThreadFunction, &updateTimeThreadData);
                pthread_create(&threads[1], NULL, &inputThreadFunction, &inputThreadData);
                pthread_create(&threads[2], NULL, &outputThreadFunction, &outputThreadData);


                pthread_join(threads[0], NULL);
                pthread_join(threads[1], NULL);
                pthread_join(threads[2], NULL);
                disable_raw_mode();
                if (sh_data->gameState == 'P') serverExists = 1;
                else serverExists = 0;
            } else {
                fprintf(stderr, "main: server uz bezi!\n");
            }
        } else if (choice == 'C' && serverExists == 1) {
            // continue existing game
            // get shared memory id
            enable_raw_mode();
            int shmid = shmget(SHM_KEY_CLIENT_SERVER, sizeof(shared_data_t), 0600);
            if (shmid < 0) {
                perror("shmget");
                return 1;
            } else {
                printf("Pripojenie k existujúcemu serveru...\n");
                void *addr = shmat(shmid, NULL, 0);
                if (addr == NULL) {
                    perror("Failed to attach shared memory block:");
                    return 1;
                }

                shared_data_t *sh_data = (shared_data_t *) addr;
                sh_data->isConnected = 1;
                sh_data->gameState = 'R'; // running
                sh_data->lastClientUpdateTime = time(NULL);
                sh_data->gameStartTime = time(NULL);
                pthread_t threads[3];

                input_th_data_t inputThreadData = {
                    &sh_data->snakeDirectionMutex, &sh_data->snakeDirection, &sh_data->gameState
                };
                output_th_data_t outputThreadData = {
                    &sh_data->updateGameFieldMutex, &sh_data->field, &sh_data->gameState, &sh_data->gameStartTime, &sh_data->snakeLength
                };
                time_update_th_data_t updateTimeThreadData = {
                    &sh_data->clientUpdateMutex, &sh_data->lastClientUpdateTime, &sh_data->gameState
                };

                printf("klient bezi\n");

                pthread_create(&threads[0], NULL, &timeClientUpdateThreadFunction, &updateTimeThreadData);
                pthread_create(&threads[1], NULL, &inputThreadFunction, &inputThreadData);
                pthread_create(&threads[2], NULL, &outputThreadFunction, &outputThreadData);


                pthread_join(threads[0], NULL);
                pthread_join(threads[1], NULL);
                pthread_join(threads[2], NULL);
                disable_raw_mode();
                if (sh_data->gameState == 'P') serverExists = 1;
                else serverExists = 0;
            }

        } else {
            printf("Invalid choice. Please try again.\n");
            continue;
        }
    }
}
