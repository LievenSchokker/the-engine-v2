cmake_minimum_required(VERSION 3.22)
project(engineSandbox)

file(GLOB BEHAVIOUR_SOURCES
        "${CMAKE_SOURCE_DIR}/Sandbox/Behaviours/*.cpp"
)

add_executable(engineSandbox
        sandbox_animation_spritesheets.cpp
        ${BEHAVIOUR_SOURCES}
)

target_link_libraries(
        engineSandbox
        PRIVATE
        Engine::Core)