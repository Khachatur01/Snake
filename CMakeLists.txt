cmake_minimum_required(VERSION 3.23)
project(snake)

set(CMAKE_CXX_STANDARD 23)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(snake
        game/game.h
        main.cpp)

target_link_libraries(snake ${CURSES_LIBRARIES})