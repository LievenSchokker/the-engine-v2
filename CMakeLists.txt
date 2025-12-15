cmake_minimum_required(VERSION 3.22)
project(program)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SDL2 2.0.18 is the minimum version required for the SDL_RenderFillRectF function
find_package(SDL2 2.0.18 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2_mixer REQUIRED CONFIG)
find_package(GameNetworkingSockets REQUIRED CONFIG)
find_package(GTest REQUIRED)

enable_testing()

add_subdirectory(Engine)
add_subdirectory(Sandbox)
add_subdirectory(Tests)
