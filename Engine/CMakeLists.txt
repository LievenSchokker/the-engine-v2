cmake_minimum_required(VERSION 3.22)
project(engine)

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES
        "${CMAKE_SOURCE_DIR}/Engine/src/*.cpp"
)

add_library(engine STATIC ${ENGINE_SOURCES})

target_include_directories(engine PUBLIC
        ${CMAKE_SOURCE_DIR}/Engine/inc
        ${CMAKE_SOURCE_DIR}/Engine/src
        ${SDL2_INCLUDE_DIRS}
)