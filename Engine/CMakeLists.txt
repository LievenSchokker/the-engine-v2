cmake_minimum_required(VERSION 3.22)
project(engine)

file(GLOB_RECURSE ENGINE_SOURCES
        "${CMAKE_SOURCE_DIR}/Engine/src/*.cpp"
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
        ${CMAKE_SOURCE_DIR}/Engine/inc
        ${CMAKE_SOURCE_DIR}/Engine/third-party
)

target_link_libraries(engine PUBLIC
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        GameNetworkingSockets::GameNetworkingSockets
        box2d
)

add_library(Engine::Core ALIAS engine)