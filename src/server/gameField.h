//
// Created by marti on 3. 1. 2026.
//

#ifndef SEMESTRALKA_GAMEFIELD_H
#define SEMESTRALKA_GAMEFIELD_H

typedef struct position {
    int x;
    int y;
    char typeOccupied; // ' ' - empty, H - snake, F - fruit, X - obstacle
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
int checkCollision(field_t *gameField, snake_position_t *snakePosition);

void initializeGameField(field_t *gameField, int lengthX, int lengthY, char randomGeneration);
void destroyGameField(field_t *gameField);

void initializeSnakePosition(snake_position_t *snakePosition, int startX, int startY, int fieldLengthX, int fieldLengthY);
void destroySnakePosition(snake_position_t *snakePosition);

#endif //SEMESTRALKA_GAMEFIELD_H