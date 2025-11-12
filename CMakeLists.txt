cmake_minimum_required(VERSION 3.22)
project(program)



set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(GTest REQUIRED)

enable_testing()

pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_TTF_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_executable(program
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/main.cpp
)

target_include_directories(program PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
)

target_link_directories(program PRIVATE
        ${SDL2_TTF_LIBRARY_DIRS}
)

target_link_libraries(program
        ${SDL2_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        CURL::libcurl
)

add_executable(unit_tests
        ${SOURCES}
        Tests/core/rendering/SDL/SDLRenderTest.cpp
)

target_include_directories(unit_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
)

target_link_directories(unit_tests PRIVATE
        ${SDL2_TTF_LIBRARY_DIRS}
)

target_link_libraries(unit_tests
        GTest::gtest
        GTest::gtest_main
        ${SDL2_LIBRARIES}
        CURL::libcurl
)

include(GoogleTest)
gtest_discover_tests(unit_tests)