//
// Created by marti on 3. 1. 2026.
//

#include <stdio.h>
#include <stdlib.h>

#include "gameConfiguration.h"

void fillGameConfiguration(game_conf_t* config) {

    char gameMode; // 'S' standard,  'T' time-limit
    int timeLimit; // in seconds if gameMode is 'T'
    char gameField; // 'N' no-obstacles, 'O' obstacles, 'C' custom
    char randomGeneration; // 'Y' yes, 'N' no` if gameField is 'O'
    int fieldLengthX; // if gameField is Y
    int fieldLengthY; // if gameField is Y
    char* customFieldPath;

    while (1) {
        printf("Select game mode : 'S' standard,  'T' time-limit\n");
        scanf("%c", &gameMode);
        if (gameMode == 'S' || gameMode == 'T') {
            config->gameMode = gameMode;
            break;
        } else {
            printf("Invalid input. Please try again.\n");
        }
    }

    if (gameMode == 'T') {
        while (1) {
            printf("Enter time limit in seconds (positive integer): ");
            scanf("%d", &timeLimit); // needs a fix to handle invalid input
            if (timeLimit > 0) {
                break;
            } else {
                printf("Invalid input. Please enter a positive integer.\n");
            }
        }
        config->timeLimit = timeLimit;
    }

    while (1) {
        printf("Select game field type: 'N' no-obstacles, 'O' obstacles, 'C' custom\n");
        scanf(" %c", &gameField);
        if (gameField == 'N' || gameField == 'O' || gameField == 'C') {
            config->gameField = gameField;
            break;
        } else {
            printf("Invalid input. Please try again.\n");
        }
    }

    if (gameField == 'O') {
        while (1) {
            printf("Random obstacle generation? 'Y' yes, 'N' no\n");
            scanf(" %c", &randomGeneration);
            if (randomGeneration == 'Y' || randomGeneration == 'N') {
                config->randomGeneration = randomGeneration;
                break;
            } else {
                printf("Invalid input. Please try again.\n");
            }
        }

        if (randomGeneration != 'C') {
            while (1) {
                printf("Enter field length X (positive integer): ");
                scanf("%d", &fieldLengthX);
                if (fieldLengthX > 0) {
                    config->fieldLengthX = fieldLengthX;
                    break;
                } else {
                    printf("Invalid input. Please enter a positive integer.\n");
                }
            }

            while (1) {
                printf("Enter field length Y (positive integer): ");
                scanf("%d", &fieldLengthY);
                if (fieldLengthY > 0) {
                    config->fieldLengthY = fieldLengthY;
                    break;
                } else {
                    printf("Invalid input. Please enter a positive integer.\n");
                }
            }
        }
    } else if (gameField == 'C') {
        customFieldPath = (char*)malloc(256 * sizeof(char));
        printf("Enter custom field file path: ");
        // todo
        scanf("%s", customFieldPath);
        config->customFieldPath = customFieldPath;
    }




}