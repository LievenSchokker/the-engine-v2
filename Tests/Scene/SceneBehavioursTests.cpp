//
// Created by samle on 28/11/2025.
//

#include <gtest/gtest.h>

#include "GameObject/GameObject.h"
#include "../Behaviour/TestBehaviours.h"
#include "Scene/Scene.h"



/// Tests to see if the scene->onStart correctly initialises the behaviours in the scene (call lifecycle functions in order).
TEST(SceneBehaviourTests, SceneInitialisesBehavioursOnStart)
{
    /// Setup.
    Scene scene {"TestScene"};
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();

    scene.addGameObject(std::move(go));

    /// Scene hasnt started yet, behaviours should not have started.
    EXPECT_FALSE(behaviour->awakeCalled);
    EXPECT_FALSE(behaviour->enableCalled);
    EXPECT_FALSE(behaviour->startCalled);
    EXPECT_FALSE(behaviour->updateCalled);

    /// Start the scene
    scene.onStart();

    /// Behaviours should now be initialised correctly.
    EXPECT_TRUE(behaviour->awakeCalled);
    EXPECT_TRUE(behaviour->enableCalled);
    EXPECT_TRUE(behaviour->startCalled);
}

/// Tests the Behaviour->Update function only calls after the Scene calls its member update.
TEST(SceneBehaviourTests, BehaviourUpdateAfterSceneUpdate)
{
    Scene scene {"TestScene"};

    // Create GO with one test behaviour
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();

    scene.addGameObject(std::move(go));

    EXPECT_FALSE(behaviour->updateCalled);
    scene.onStart();
    EXPECT_FALSE(behaviour->updateCalled);

    scene.update(0.016f);
    EXPECT_TRUE(behaviour->updateCalled);
}

TEST(SceneBehaviourTests, ActiveSceneInitialisesNewBehaviours)
{
    Scene scene {"TestScene"};

    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();

    /// Expect all false; scene doesnt have GO and hasnt started yet.
    EXPECT_FALSE(behaviour->awakeCalled);
    EXPECT_FALSE(behaviour->enableCalled);
    EXPECT_FALSE(behaviour->startCalled);
    EXPECT_FALSE(behaviour->updateCalled);

    scene.onStart();
    scene.update(0.016f);

    /// Expect all false; Scene does not have the GO yet.
    EXPECT_FALSE(behaviour->awakeCalled);
    EXPECT_FALSE(behaviour->enableCalled);
    EXPECT_FALSE(behaviour->startCalled);
    EXPECT_FALSE(behaviour->updateCalled);

    /// Now add the GO to the scene
    scene.addGameObject(std::move(go));
    scene.update(0.016f); /// Call Update manually, normally handled by SceneManager,

    /// Expect all true; GO has been added to a scene that has started.
    EXPECT_TRUE(behaviour->awakeCalled);
    EXPECT_TRUE(behaviour->enableCalled);
    EXPECT_TRUE(behaviour->startCalled);
    EXPECT_TRUE(behaviour->updateCalled);
}

TEST(SceneBehaviourTests, AddGameObjectToDestroyQueue)
{
    // Scene scene {"TestScene"};
    //
    // auto go = std::make_unique<GameObject>();
    // go->setName("TestObject");
    // auto* goPtr = go.get();
    //
    // ASSERT_EQ(goPtr->getName(), "TestObject");
    // auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();

}

