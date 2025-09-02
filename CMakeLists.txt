cmake_minimum_required(VERSION 3.10)
project(Chess C)

set(CMAKE_C_STANDARD 99)

# Set build type to Debug if not defined
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Set Debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files for main executable
set(SOURCE_FILES
    src/main.c
    src/board.c
    src/misc.c
    src/perft.c
    src/search.c
    src/magic_numbers.c
)

# Create main executable
add_executable(chess ${SOURCE_FILES})

# Add a CMake option to enable/disable testing
option(ENABLE_TESTS "Enable Unit Tests" OFF)

if(ENABLE_TESTS)
    message(STATUS "Building with tests enabled")

    # Enable testing framework
    enable_testing()

    # Add Unity (Testing Framework)
    add_library(Unity STATIC Unity/src/unity.c)

    # Test files
    set(TEST_FILES tests/test_board.c)
	
set(B_SOURCE_FILES

    src/board.c
    src/misc.c
    src/perft.c
    src/search.c
    src/magic_numbers.c
)
    # Create test executable
	set(BOARD_SOURCES src/board.c)
	add_executable(test_runner tests/test_board.c ${B_SOURCE_FILES} Unity/src/unity.c)
    target_link_libraries(test_runner Unity)

    # Register test
    add_test(NAME ChessTests COMMAND test_runner)
else()
    message(STATUS "Tests are disabled")
endif()
