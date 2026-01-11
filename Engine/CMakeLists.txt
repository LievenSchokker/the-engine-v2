cmake_minimum_required(VERSION 3.22)
project(engine)

file(GLOB_RECURSE ENGINE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
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
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/cereal/include
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(GameNetworkingSockets CONFIG REQUIRED)

target_link_libraries(engine PUBLIC
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
        GameNetworkingSockets::GameNetworkingSockets
        box2d
)

add_library(Engine::Core ALIAS engine)

# Doxygen documentation
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
    set(DOXYGEN_PROJECT_NAME "Engine Documentation")
    set(DOXYGEN_PROJECT_BRIEF "A modern C++20 game engine")
    set(DOXYGEN_INPUT_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/inc)
    set(DOXYGEN_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/../Readme.md)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/../Readme.md)
    
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
    message(STATUS "Doxygen found - documentation target 'docs' available")
else()
    message(STATUS "Doxygen not found - documentation target will not be available")
    if(WIN32)
        message(STATUS "Install Doxygen: vcpkg install doxygen (or download from doxygen.org)")
    else()
        message(STATUS "Install Doxygen: brew install doxygen (macOS) or apt install doxygen (Linux)")
    endif()
endif()