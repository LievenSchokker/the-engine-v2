add_executable(engine_ai_tests
        tests_agent.cpp
        TestAgentModule.cpp
        tests_navigation_grid.cpp
        tests_boundingBox.cpp
        tests_pathResult.cpp
        tests_aStarPathfinder.cpp
        tests_gridGraphAdapter.cpp
        tests_gridComponentNavigationSurface.cpp
        tests_navigationSystem.cpp
)

target_link_libraries(engine_ai_tests PRIVATE
        Engine::Core
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(engine_ai_tests)
