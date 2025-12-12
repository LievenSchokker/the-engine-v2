cmake_minimum_required(VERSION 3.22)
project(engineSandbox)

file(GLOB BEHAVIOUR_SOURCES
        "${CMAKE_SOURCE_DIR}/Sandbox/Behaviours/*.cpp"
)


add_executable(engineSandbox
        physics.cpp
        ${BEHAVIOUR_SOURCES}
)

add_custom_command(
        TARGET engineSandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Sandbox/Assets
        ${CMAKE_CURRENT_BINARY_DIR}/Assets
)

target_link_libraries(
        engineSandbox
        PRIVATE
        Engine::Core)