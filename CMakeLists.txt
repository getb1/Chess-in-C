cmake_minimum_required(VERSION 3.0)
project(Chess)

set(CMAKE_C_STANDARD 99)

include_directories(${CMAKE_SOURCE_DIR}/src)



set(SOURCE_FILES
        src/main.c
        src/board.c
        src/misc.c
        src/perft.c)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
add_executable(main ${SOURCE_FILES})