cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/Users/jaedontaylor/Desktop/minesweeper")
project(minesweeper)


set(SFML_DIR "/opt/homebrew/lib/cmake/SFML")
add_executable(minesweeper main.cpp minesweeper.h taylor.jaedon.project3/minesweeper.h taylor.jaedon.project3/main.cpp)
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)
target_link_libraries(minesweeper sfml-system sfml-window sfml-graphics)

