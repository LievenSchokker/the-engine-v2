cmake_minimum_required(VERSION 3.22)
project(engineSandbox)

add_executable(engineSandbox
        sandbox_parent_child.cpp
)

target_link_libraries(
        engineSandbox
        PRIVATE
        Engine::Core)