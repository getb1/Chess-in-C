cmake_minimum_required(VERSION 3.0)
project(Chess)

set(CMAKE_C_STANDARD 99)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCE_FILES
        src/main.c
        src/board.c
        src/misc.c)

add_executable(main ${SOURCE_FILES})