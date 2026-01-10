//
// Created by marti on 3. 1. 2026.
//

#include "gameField.h"
#include "../shared/sharedData.h"
#include <stdlib.h>




void initializeGameField(field_t *gameField, int lengthX, int lengthY, char randomGeneration) {
    gameField->fieldLengthX = lengthX;
    gameField->fieldLengthY = lengthY;

    gameField->positions = (position_t **)malloc(lengthY * sizeof(position_t *));
    for (int i = 0; i < lengthY; i++) {
        gameField->positions[i] = (position_t *)malloc(lengthX * sizeof(position_t));
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
    free(gameField->positions);
    gameField->positions = NULL;
    gameField->fieldLengthX = 0;
    gameField->fieldLengthY = 0;
}

void initializeSnakePosition(snake_position_t *snakePosition, int startX, int startY, int fieldLengthX, int fieldLengthY) {
    snakePosition->length = 1;
    snakePosition->positions = (position_t *)malloc(fieldLengthY * fieldLengthX * sizeof(position_t)); // Max possible length
    snakePosition->positions[0].x = startX;
    snakePosition->positions[0].y = startY;
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

int moveSnake(field_t *gameField, snake_position_t *snakePosition, char direction) {
    // Calculate new head position
    int newX = snakePosition->positions[0].x;
    int newY = snakePosition->positions[0].y;

    switch (direction) {
        case DIRECTION_UP:    newY--; break;
        case DIRECTION_DOWN:  newY++; break;
        case DIRECTION_LEFT:  newX--; break;
        case DIRECTION_RIGHT: newX++; break;
        default: return -1; // Invalid direction
    }

    newY %= gameField->fieldLengthY;
    newX %= gameField->fieldLengthX;
    if (newY < 0) newY += gameField->fieldLengthY;
    if (newX < 0) newX += gameField->fieldLengthX;

    if (gameField->positions[newY][newX].typeOccupied == 'F') {
        // Eat fruit: increase snake length
        snakePosition->length++;

        // Place new head at the front
        for (int i = snakePosition->length - 1; i > 0; i--) {
            snakePosition->positions[i] = snakePosition->positions[i - 1];
        }
        snakePosition->positions[0].x = newX;
        snakePosition->positions[0].y = newY;
        gameField->positions[newY][newX].typeOccupied = ' ';
        spawnFruit(gameField);
        return 1;
    }

    // Move snake positions
    for (int i = snakePosition->length - 1; i > 0; i--) {
        snakePosition->positions[i] = snakePosition->positions[i - 1];
    }
    snakePosition->positions[0].x = newX;
    snakePosition->positions[0].y = newY;

    // Check for collisions
    if (checkCollision(gameField, snakePosition)) {
        return -1; // Collision detected
    }

    return 0; // Successful move
}

int checkCollision(field_t *gameField, snake_position_t *snakePosition) {
    int headX = snakePosition->positions[0].x;
    int headY = snakePosition->positions[0].y;

    // Check obstacle collisions
    if (gameField->positions[headY][headX].typeOccupied == 'X') {
        return 1; // Collision with obstacle
    }

    // Check self-collisions
    for (int i = 1; i < snakePosition->length; i++) {
        if (snakePosition->positions[i].x == headX && snakePosition->positions[i].y == headY) {
            return 1; // Collision with itself
        }
    }

    return 0; // No collision
}
