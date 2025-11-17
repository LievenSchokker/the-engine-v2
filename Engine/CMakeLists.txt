cmake_minimum_required(VERSION 3.22)
project(engine)

file(GLOB_RECURSE ENGINE_SOURCES
        "${CMAKE_SOURCE_DIR}/Engine/src/*.cpp"
)

add_library(engine STATIC ${ENGINE_SOURCES})

target_include_directories(engine PUBLIC
        ${CMAKE_SOURCE_DIR}/Engine/inc
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${GAMENETWORKINGSOCKETS_INCLUDE_DIRS}
)

target_link_libraries(engine PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        GameNetworkingSockets::GameNetworkingSockets
        Box2D
)

add_library(Engine::Core ALIAS engine)