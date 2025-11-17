cmake_minimum_required(VERSION 3.22)
project(engine_tests)

find_package(GTest REQUIRED)

add_executable(unit_tests
        Rendering/SDL/SDLRenderTest.cpp
        Input/InputManagerTest.cpp
)

target_link_libraries(unit_tests PRIVATE
        engine
        GTest::gtest
        GTest::gtest_main
        SDL2::SDL2
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
