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

add_library(engine STATIC ${ENGINE_SOURCES})

target_include_directories(engine PUBLIC
        ${CMAKE_SOURCE_DIR}/Engine/inc
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${GAMENETWORKINGSOCKETS_INCLUDE_DIRS}
)

target_compile_definitions(engine PUBLIC SDL_MAIN_HANDLED)

target_link_libraries(engine PUBLIC
        SDL2::SDL2
        SDL2_image::SDL2_image
        GameNetworkingSockets::GameNetworkingSockets
        box2d
)

add_library(Engine::Core ALIAS engine)