cmake_minimum_required(VERSION 3.22)
project(program)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(SDL2 REQUIRED)

enable_testing()

add_subdirectory(Engine)
add_subdirectory(Sandbox)
add_subdirectory(Tests)