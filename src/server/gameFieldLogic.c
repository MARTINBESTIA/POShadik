//
// Created by marti on 3. 1. 2026.
//

#include "gameFieldLogic.h"
#include "../shared/sharedData.h"
#include <stdlib.h>




void initializeGameField(field_t *gameField, int lengthX, int lengthY, char randomGeneration) {

    gameField->fieldLengthX = lengthX;
    gameField->fieldLengthY = lengthY;

    for (int i = 0; i < lengthY; i++) {
        for (int j = 0; j < lengthX; j++) {
            gameField->positions[i][j].x = j;
            gameField->positions[i][j].y = i;
            gameField->positions[i][j].typeOccupied = ' ';
        }
    }

    // Add obstacles if randomGeneration is 'Y'
    // Might leave unreachable spaces needs rework
    if (randomGeneration == 'Y') {
        int numObstacles = (lengthX * lengthY) / 10; // Example: 10% of the field
        for (int n = 0; n < numObstacles; n++) {
            int x = rand() % lengthX;
            int y = rand() % lengthY;
            gameField->positions[y][x].typeOccupied = 'X'; // 'X' for obstacle
        }
    }
}

void destroyGameField(field_t *gameField) {
    for (int i = 0; i < gameField->fieldLengthY; i++) {
        free(gameField->positions[i]);
    }
    gameField->fieldLengthX = 0;
    gameField->fieldLengthY = 0;
}

void initializeSnakePosition(snake_position_t *snakePosition, int startX, int startY, int fieldLengthX, int fieldLengthY) {
    snakePosition->length = 1;
    printf("Snake params: startX: %d, startY: %d, fieldLengthX: %d, fieldLengthY: %d\n", startX, startY, fieldLengthX, fieldLengthY);
    snakePosition->positions = (position_t *)malloc(fieldLengthY * fieldLengthX * sizeof(position_t)); // Max possible length
    snakePosition->positions[0].x = startX;
    snakePosition->positions[0].y = startY;
    if (startX == 0 || startY == 0) {
        snakePosition->positions[0].x = 0;
        snakePosition->positions[0].y = 0;
    }
    snakePosition->positions[0].typeOccupied = 'H'; // 'H' for head of the snake
}

void destroySnakePosition(snake_position_t *snakePosition) {
    free(snakePosition->positions);
    snakePosition->positions = NULL;
    snakePosition->length = 0;
}

void spawnFruit(field_t *gameField) {
    int x, y;
    do {
        x = rand() % gameField->fieldLengthX;
        y = rand() % gameField->fieldLengthY;
    } while (gameField->positions[y][x].typeOccupied != ' '); // Find an empty spot

    gameField->positions[y][x].typeOccupied = 'F'; // 'F' for fruit
}

int moveSnake(field_t *gameField, snake_position_t *snakePosition, char direction, int* snakeLengthPtr) {
    // Calculate new head position
    int newX = snakePosition->positions[0].x;
    int newY = snakePosition->positions[0].y;
    switch (direction) {
        case DIRECTION_UP: newY--; break;
        case DIRECTION_DOWN: newY++; break;
        case DIRECTION_LEFT: newX--; break;
        case DIRECTION_RIGHT: newX++; break;
        default: return -1; // Invalid direction
    }
    newY %= gameField->fieldLengthY;
    newX %= gameField->fieldLengthX;
    if (newY < 0) newY += gameField->fieldLengthY;
    if (newX < 0) newX += gameField->fieldLengthX;

    if (gameField->positions[newY][newX].typeOccupied == 'X') return -1; // Collision with obstacle

    int ateFood = 0;
    if (gameField->positions[newY][newX].typeOccupied == 'F') ateFood++;
    int lastX = snakePosition->positions[snakePosition->length - 1].x;
    int lastY = snakePosition->positions[snakePosition->length - 1].y;
    for (int i = snakePosition->length - 1; i > 0; i--) {
        snakePosition->positions[i] = snakePosition->positions[i - 1];

        // Update field for body parts
        int x = snakePosition->positions[i].x;
        int y = snakePosition->positions[i].y;
        gameField->positions[y][x].typeOccupied = 'H'; // Body
    }
    if (!ateFood) {
        gameField->positions[lastY][lastX].typeOccupied = ' ';
    }

    snakePosition->positions[0].x = newX;
    snakePosition->positions[0].y = newY;
    gameField->positions[newY][newX].typeOccupied = 'H'; // 'H' for snake head

    if (ateFood) {
        snakePosition->positions[snakePosition->length].x = lastX;
        snakePosition->positions[snakePosition->length].y = lastY;
        gameField->positions[lastY][lastX].typeOccupied = 'H';
        snakePosition->length++;
        (*snakeLengthPtr)++;
        spawnFruit(gameField);
    }
    // Check for collisions
    if (checkCollision(gameField, snakePosition)) {
        return -1; // Collision detected
    }
    return 0; // Successful move
}

int checkCollision(field_t *gameField, snake_position_t *snakePosition) {
    int headX = snakePosition->positions[0].x;
    int headY = snakePosition->positions[0].y; // Check obstacle collisions
    if (gameField->positions[headY][headX].typeOccupied == 'X') {
        return 1; // Collision with obstacle
    } // Check self-collisions
    for (int i = 1; i < snakePosition->length; i++) {
        if (snakePosition->positions[i].x == headX && snakePosition->positions[i].y == headY) {
            return 1; // Collision with itself
        }
    }
    return 0; // No collision
}
