cmake_minimum_required(VERSION 3.22)
project(engine)

file(GLOB_RECURSE ENGINE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

include(FetchContent)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v3.1.1
)

FetchContent_MakeAvailable(box2d)

add_library(engine STATIC
        third-party/nuklearImplemntation.cpp
        ${ENGINE_SOURCES})

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/cereal/include
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(GameNetworkingSockets CONFIG REQUIRED)

target_link_libraries(engine PUBLIC
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        GameNetworkingSockets::GameNetworkingSockets
        box2d
)

add_library(Engine::Core ALIAS engine)