cmake_minimum_required(VERSION 3.14)
project(rpg_engine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directory
include_directories(include)

# Source files for core library/game
set(SOURCES
    src/Hero.cpp
    src/Enemy.cpp
    src/Item.cpp
    src/Inventory.cpp
    src/CombatEngine.cpp
    src/StoryGraph.cpp
    src/SaveManager.cpp
)

# Core engine static library
add_library(rpg_core STATIC ${SOURCES})

# Main executable
add_executable(rpg_engine src/main.cpp)
target_link_libraries(rpg_engine PRIVATE rpg_core)

# Tests setup
enable_testing()

file(GLOB TEST_FILES "tests/test_*.cpp")
foreach(TEST_SRC ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_link_libraries(${TEST_NAME} PRIVATE rpg_core)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
