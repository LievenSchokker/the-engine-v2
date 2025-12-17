cmake_minimum_required(VERSION 3.22)
project(engineSandbox)

# Glob the Behaviour sources
file(GLOB BEHAVIOUR_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Behaviours/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Behaviours/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Examples/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Examples/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Sandboxes/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Sandboxes/*.h"
)

# Helper function to create a sandbox executable
function(add_sandbox_executable name source_file)
    add_executable(${name}
            ${source_file}
            ${BEHAVIOUR_SOURCES}
    )

    # Set include directories so includes work from Sandbox root
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # Copy the Assets directory to the binary directory
    add_custom_command(
            TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/Assets
            ${CMAKE_CURRENT_BINARY_DIR}/Assets
    )

    # Link the library to the executable
    target_link_libraries(
            ${name}
            PRIVATE
            Engine::Core)
endfunction()

# Create separate executables for each sandbox
# (You can comment out the executables you don't want to build.)
add_sandbox_executable(engineSandbox_main main.cpp)
add_sandbox_executable(engineSandbox_physics Physics.cpp)
add_sandbox_executable(engineSandbox_animation_spritesheets sandbox_animation_spritesheets.cpp)
add_sandbox_executable(engineSandbox_animation_transforms sandbox_animation_transforms.cpp)
add_sandbox_executable(engineSandbox_input sandbox_input.cpp)
add_sandbox_executable(engineSandbox_scene sandbox_scene.cpp)
add_sandbox_executable(engineSandbox_tilemap_and_grid sandbox_tilemap_and_grid.cpp)
add_sandbox_executable(engineSandbox_debug_time_controls sandbox_debug_time_controls.cpp)
add_sandbox_executable(engineSandbox_audio sandbox_audio.cpp)