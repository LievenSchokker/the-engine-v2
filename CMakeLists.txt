cmake_minimum_required(VERSION 3.22)
project(program)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(GTest REQUIRED)

enable_testing()

file(GLOB_RECURSE ENGINE_SOURCES
        "${CMAKE_SOURCE_DIR}/Engine/src/*.cpp"
)

add_library(engine STATIC
        ${ENGINE_SOURCES}
)

target_include_directories(engine PUBLIC
        ${CMAKE_SOURCE_DIR}/Engine/inc
        ${CMAKE_SOURCE_DIR}/Engine/src
        ${SDL2_INCLUDE_DIRS}
)

set(SANDBOX_MAIN "${CMAKE_SOURCE_DIR}/Sandbox/main.cpp")

add_executable(program
        ${SANDBOX_MAIN}
)

target_include_directories(program PRIVATE
        ${CMAKE_SOURCE_DIR}/Engine/inc
        ${CMAKE_SOURCE_DIR}/Engine/src
        ${SDL2_INCLUDE_DIRS}
)

target_link_directories(program PRIVATE
        ${SDL2_TTF_LIBRARY_DIRS}
)

target_link_libraries(program PRIVATE
        engine
        ${SDL2_LIBRARIES}
)

add_executable(unit_tests
        Tests/Rendering/SDL/SDLRenderTest.cpp
        Tests/Input/InputManagerTest.cpp
)

target_link_libraries(unit_tests PRIVATE
        engine
        GTest::gtest
        GTest::gtest_main
        ${SDL2_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(unit_tests)