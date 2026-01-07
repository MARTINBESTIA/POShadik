add_library(pipeSocketLib SHARED)
target_sources(pipeSocketLib PRIVATE pipe.c socket.c)
target_compile_options(pipeSocketLib PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
target_include_directories(pipeSocketLib PUBLIC ..)
