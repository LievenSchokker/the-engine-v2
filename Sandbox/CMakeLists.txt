cmake_minimum_required(VERSION 3.22)
project(engineSandbox)

add_executable(engineSandbox
        main.cpp
)

target_link_libraries(
        engineSandbox
        PRIVATE
        Engine::Core)