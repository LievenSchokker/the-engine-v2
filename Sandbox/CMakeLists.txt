cmake_minimum_required(VERSION 3.22)
project(engineSandbox)

add_executable(engineSandbox
        main.cpp
        SimpleMoveBehaviour.cpp
        Behaviours/NavigationTest.cpp
        AgentsZandbak.cpp
)

target_link_libraries(
        engineSandbox
        PRIVATE
        Engine::Core)