//
// Created by marti on 4. 1. 2026.
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../shared/gameConfiguration.h"
#include "gameField.h"

int main(int argc, char** argv) {

    game_conf_t game_conf;
    read(0, &game_conf, sizeof(game_conf_t));
    close(0);
    printf("Game mode: %c\n", game_conf.gameMode);
    printf("Time limit: %d\n", game_conf.timeLimit);
    printf("Game field: %c\n", game_conf.gameField);
    if (game_conf.gameField == 'O') {
        printf("Random generation: %c\n", game_conf.randomGeneration);
        printf("Field length X: %d\n", game_conf.fieldLengthX);
        printf("Field length Y: %d\n", game_conf.fieldLengthY);
    } else if (game_conf.gameField == 'C') {
        printf("Custom field path: %s\n", game_conf.customFieldPath);

    }

    return 0;
}
