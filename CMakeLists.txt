cmake_minimum_required(VERSION 3.22)
project(program)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(GTest REQUIRED)

enable_testing()

file(GLOB_RECURSE SOURCES
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_executable(program
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/main.cpp
)

target_include_directories(program PRIVATE
        ${CMAKE_SOURCE_DIR}/inc
        ${CMAKE_SOURCE_DIR}/src
        ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(program PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
)

add_executable(unit_tests
        ${SOURCES}
        Tests/Rendering/SDL/SDLRenderTest.cpp
        Tests/Input/InputManagerTest.cpp
)

target_include_directories(unit_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/inc
        ${CMAKE_SOURCE_DIR}/src
        ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(unit_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
)

include(GoogleTest)
gtest_discover_tests(unit_tests)