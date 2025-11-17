cmake_minimum_required(VERSION 3.22)
project(program)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)
find_package(GameNetworkingSockets REQUIRED)
find_package(GTest REQUIRED)

include(FetchContent)

FetchContent_Declare(
        Box2D
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v3.1.1
)

FetchContent_MakeAvailable(Box2D)

enable_testing()

add_subdirectory(Engine)
add_subdirectory(Sandbox)
add_subdirectory(Tests)