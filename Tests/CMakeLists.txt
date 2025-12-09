cmake_minimum_required(VERSION 3.22)
project(engine_tests)

add_executable(engine_tests
        Rendering/SDL/SDLRendererTest.cpp
        Rendering/ShapeRendererTest.cpp
        Rendering/UserInterface/TextCommandTest.cpp
        Input/InputManagerTest.cpp
        Asset/AssetManagerTest.cpp
        Scene/SceneManagerTest.cpp
        Core/SpelMotorTest.cpp
        Core/ServerLoopTest.cpp
        Core/ClientLoopTest.cpp
        Core/TimerTest.cpp
        Component/Profiler/ProfilerTest.cpp
        Component/tests_TestComponents.cpp
        Component/TestComponentBase.cpp
        Component/tests_component.cpp
        Component/tests_component_manager.cpp
        Component/GridComponentTest.cpp
        Scene/SceneManagerTest.cpp
        Networking/Messages/MessageReaderTest.cpp
        Networking/Messages/MessageWriterTest.cpp
        Networking/Messages/MessageDispatcherTest.cpp
        Networking/ServerTest.cpp
        Behaviour/TestBehaviourBase.cpp
        Behaviour/tests_behaviour.cpp
        Math/TestsVector2.cpp
        Scene/SceneManagerTest.cpp
        Physics/PhysicsSystemTest.cpp
        Animation/AnimationCurveTest.cpp
        Animation/AnimationTrackTest.cpp
        Animation/AnimationClipTest.cpp
        Animation/AnimatorTest.cpp
        Scene/SceneBehavioursTests.cpp
)

target_link_libraries(engine_tests PRIVATE
        Engine::Core
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(engine_tests)
