cmake_minimum_required(VERSION 3.7)
project(LinearWang)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")

set(SOURCE_FILES src/main.cpp src/general.cpp src/general.h src/board.cpp src/board.h src/coloring.h src/wang.cpp src/wang.h src/cycle_solver.cpp src/cycle_solver.h src/output.cpp src/output.h src/tree_solver.cpp src/tree_solver.h)
add_executable(LinearWang ${SOURCE_FILES})