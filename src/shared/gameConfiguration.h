//
// Created by marti on 3. 1. 2026.
//

#ifndef SEMESTRALKA_GAMECONFIGURATION_H
#define SEMESTRALKA_GAMECONFIGURATION_H

typedef struct GameConfiguration {
    char gameMode; // 'S' standard,  'T' time-limit
    int timeLimit; // in seconds if gameMode is 'T'
    char gameField; // 'N' no-obstacles, 'O' obstacles, 'C' custom
    char randomGeneration; // 'Y' yes, 'N' no` if gameField is 'O'
    int fieldLengthX; // if gameField is Y
    int fieldLengthY; // if gameField is Y
    char* customFieldPath; // if gameField is 'C'
} game_conf_t;


#endif //SEMESTRALKA_GAMECONFIGURATION_H