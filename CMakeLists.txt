cmake_minimum_required(VERSION 4.1)
project(semestralka C)

set(CMAKE_C_STANDARD 11)

add_executable(semestralka src/main.c
        src/shared/gameConfiguration.c
        src/shared/gameConfiguration.h
        src/server/gameField.h
        src/server/gameField.c
        src/client/clientMain.c
        src/server/gameFieldGenerator.h
        src/server/gameFieldGenerator.c)
