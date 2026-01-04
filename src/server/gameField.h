//
// Created by marti on 3. 1. 2026.
//

#ifndef SEMESTRALKA_GAMEFIELD_H
#define SEMESTRALKA_GAMEFIELD_H

typedef struct position {
    int x;
    int y;
    int typeOccupied; // 0 - empty, 1 - snake, 2 - fruit, 3 - obstacle
} position_t;

typedef struct snakePosition {
    position_t* positions;
    int length;
} snake_position_t;

typedef struct field {
    position_t** positions;
    int fieldLengthX;
    int fieldLengthY;
} field_t;

void spawnFruit(field_t *gameField);
int moveSnake(field_t *gameField, snake_position_t *snakePosition, char direction);

#endif //SEMESTRALKA_GAMEFIELD_H