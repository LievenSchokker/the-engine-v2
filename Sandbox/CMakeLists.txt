cmake_minimum_required(VERSION 3.22)
project(engineSandbox)

file(GLOB_RECURSE SANDBOX_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Behaviours/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Behaviours/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Examples/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Examples/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Sandboxes/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Sandboxes/*.h"
)

add_executable(engineSandbox
        main.cpp
        ${SANDBOX_SOURCES}
)
target_include_directories(engineSandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engineSandbox PRIVATE Engine::Core)

add_custom_command(TARGET engineSandbox
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Assets
        ${CMAKE_CURRENT_BINARY_DIR}/Assets
)