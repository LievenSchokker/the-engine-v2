cmake_minimum_required(VERSION 3.2...4.0)
project(engine)

set(CMAKE_CXX_STANDARD 20)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# Google Test
find_package(GTest REQUIRED)
enable_testing()

file(GLOB_RECURSE SRC
        "src/*.cpp"
)

# Exclude main.cpp from test sources
list(REMOVE_ITEM SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
)

# Main executable
add_executable(engine src/main.cpp ${SRC})
target_link_libraries(engine SDL2::SDL2 SDL2_image::SDL2_image)

# Test executable
file(GLOB_RECURSE TEST_SOURCES
        "Tests/*.cpp"
)

add_executable(engine_tests ${SRC} ${TEST_SOURCES})
target_include_directories(engine_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(engine_tests
        PRIVATE
        GTest::gtest
        GTest::gtest_main
        SDL2::SDL2
        SDL2_image::SDL2_image
)

include(GoogleTest)
gtest_discover_tests(engine_tests)
