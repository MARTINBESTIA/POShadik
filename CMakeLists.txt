cmake_minimum_required(VERSION 4.2.1)
project(semestralka C)

set(CMAKE_C_STANDARD 11)


add_executable(client
        src/shared/gameConfiguration.h
        src/shared/gameFieldGenerator.c
        src/shared/gameFieldGenerator.h
        src/client/clientMain.c)

add_executable(server
        src/shared/gameConfiguration.h
        src/server/gameField.h
        src/server/gameField.c
        src/shared/gameFieldGenerator.h
        src/shared/gameFieldGenerator.c
        src/server/serverMain.c)