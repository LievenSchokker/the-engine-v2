cmake_minimum_required(VERSION 3.22)
project(engineSandbox)

find_package(SDL2 REQUIRED)

add_executable(engineSandbox main.cpp)

target_link_libraries(engineSandbox PRIVATE engine)
target_link_libraries(engineSandbox PRIVATE SDL2::SDL2)