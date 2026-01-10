cmake_minimum_required(VERSION 4.2.1)
project(semestralka C)

set(CMAKE_C_STANDARD 11)


add_executable(client
        src/shared/gameConfiguration.h
        src/shared/gameConfiguration.c
        src/client/clientMain.c
        src/client/inputManager.c
        src/client/inputManager.h
        src/shared/sharedData.h
        src/shared/sharedData.c
        src/client/gameInitializer.h
        src/client/gameInitializer.c
        src/client/clientSharedInterface.h
        src/client/clientSharedInterface.c)

add_executable(server
        src/shared/gameConfiguration.h
        src/server/gameField.h
        src/server/gameField.c
        src/shared/gameConfiguration.c
        src/server/serverMain.c
        src/shared/sharedData.h
        src/shared/sharedData.c
        src/server/serverSharedInterface.h
        src/server/serverSharedInterface.c)