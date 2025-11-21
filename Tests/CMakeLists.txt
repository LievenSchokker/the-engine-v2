cmake_minimum_required(VERSION 3.22)
project(engine_tests)

add_executable(engine_tests
        Rendering/SDL/SDLRenderTest.cpp
        Input/InputManagerTest.cpp
        Component/tests_component.cpp
        Component/tests_component_manager.cpp
        GameObject/tests_game_object.cpp
        Scene/SceneManagerTest.cpp
)

target_link_libraries(engine_tests PRIVATE
        Engine::Core
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(engine_tests)
