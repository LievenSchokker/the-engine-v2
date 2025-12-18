//
// Created by samle on 28/11/2025.
//

#include "Game.h"

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
	std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    // Create GO with one test behaviour
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();

    scene.addGameObject(std::move(go));

    EXPECT_FALSE(behaviour->updateCalled);
    scene.onStart();
    EXPECT_FALSE(behaviour->updateCalled);

    scene.update(0.016f, *gameWorld.get());
    EXPECT_TRUE(behaviour->updateCalled);
}

/// This tests if an already active and running scene correctly initilises now Behaviours when adding a GameObject during running scene.
TEST(SceneBehaviourTests, ActiveSceneInitialisesNewBehaviours)
{
    Scene scene {"TestScene"};

	std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();

    /// Expect all false; scene doesnt have GO and hasnt started yet.
    EXPECT_FALSE(behaviour->awakeCalled);
    EXPECT_FALSE(behaviour->enableCalled);
    EXPECT_FALSE(behaviour->startCalled);
    EXPECT_FALSE(behaviour->updateCalled);

    /// Start and update the scene
    scene.onStart();
    scene.update(0.016f, *gameWorld.get());

    /// Expect all false; Scene does not have the GO yet.
    EXPECT_FALSE(behaviour->awakeCalled);
    EXPECT_FALSE(behaviour->enableCalled);
    EXPECT_FALSE(behaviour->startCalled);
    EXPECT_FALSE(behaviour->updateCalled);

    /// Now add the GO to the scene
    scene.addGameObject(std::move(go));
    scene.update(0.016f, *gameWorld.get()); /// Call Update manually, normally handled by SceneManager,

    /// Expect all true; GO has been added to a scene that has started, its behaviours should be initialised correctly now.
    EXPECT_TRUE(behaviour->awakeCalled);
    EXPECT_TRUE(behaviour->enableCalled);
    EXPECT_TRUE(behaviour->startCalled);
    EXPECT_TRUE(behaviour->updateCalled);
}


/// This tests if the Update function is only being run and executed on behaviours attached to an active gameobject (gameObject->getIsActive())
TEST(SceneBehaviourTests, UpdateOnlyActiveGameObjectBehaviours)
{
	std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    /// Setup:
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();
    scene.addGameObject(std::move(go));
    scene.onStart();
    scene.update(0.016f, *gameWorld.get());

    /// Scene has started and updated, behaviours should be updated as well.
    ASSERT_TRUE(behaviour->updateCalled);

    /// Deactivate the GameObject
    behaviour->getGameObject()->setActive(false);
    behaviour->updateCalled = false; /// Manually set to false so we can use it again.

    /// Update the scene again, which should NOT update behaviours on the now deactivated GO.
    scene.update(0.016f, *gameWorld.get());

    /// Should still be false, GO is not active.
    ASSERT_FALSE(behaviour->updateCalled);
}


/// This tests if the Update function is only being run and executed on behaviours that are enabled (behaviour->getIsEnabled())
TEST(SceneBehaviourTests, UpdateOnlyEnabledBehaviours)
{
    /// Setup:
    Scene scene{"TestScene"};
	std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();
    scene.addGameObject(std::move(go));
    scene.onStart();
    scene.update(0.016f, *gameWorld.get());

    /// Update should be called on behaviours.
    ASSERT_TRUE(behaviour->updateCalled);

    /// Disable the behaviour, reset its test bool
    behaviour->setEnabled(false);
    behaviour->updateCalled = false;

    /// Update the scene again, should now skip the disabled behaviour's update
    scene.update(0.016f, *gameWorld.get());

    /// Behaviour should not be updated, it is disabled.
    ASSERT_FALSE(behaviour->updateCalled);
}


/// This tests to see if the gameObject->destroy() function correctly adds it to the scene's destroyQueue in order to be destroyed.
TEST(SceneBehaviourTests, DestroyedGameObjectGoesToDestroyQueue)
{
    /// Setup:
    Scene scene{"TestScene"};
    std::unique_ptr<GameObject> go = std::make_unique<GameObject>();
    GameObject* goRaw = go.get();
    scene.addGameObject(std::move(go));

    /// Call destroy on the GameObject, which should add it to the destroyqueue of the scene.
    goRaw->destroy();

    /// Should be in the destroyQueue.
    EXPECT_TRUE(scene.isInDestroyQueue(goRaw));
}


/// This tests if the scene's destroyQueue is correctly being processed each frame, destroying the objects inside it.
TEST(SceneBehaviourTests, SceneProcessesDestroyQueueEachFrame)
{
    /// Setup:
    Scene scene{"TestScene"};
	std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    std::unique_ptr<GameObject> go = std::make_unique<GameObject>();
    go->setName("ToDestroy");
    GameObject* goRaw = go.get();
    scene.addGameObject(std::move(go));
    scene.onStart();
    scene.update(0.016f, *gameWorld.get());

    /// Should not be in the Q yet, no destroy() function called.
    EXPECT_FALSE(scene.isInDestroyQueue(goRaw));

    /// Should add it to the Q after calling go->destroy()
    goRaw->destroy();
    EXPECT_TRUE(scene.isInDestroyQueue(goRaw));

    /// scene.update processes the queue each frame, so call update here manually.
    scene.update(0.016f, *gameWorld.get());

    /// The GO should now be deleted, and be removed from the Q.
    EXPECT_FALSE(scene.isInDestroyQueue(goRaw));

    /// Check to see if the GO still exists in the scene itself.
    EXPECT_EQ(scene.getGameObject("ToDestroy"), nullptr);
}

TEST(SceneBehaviourTests, DestroyCallsOnDisableBeforeRemoval)
{
    /// Setup:
    Scene scene{"TestScene"};
	std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    auto go = std::make_unique<GameObject>();
    GameObject* goRaw = go.get();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();
    go->setName("ToDestroy");
    scene.addGameObject(std::move(go));
    scene.onStart();

    EXPECT_FALSE(behaviour->disableCalled);
    goRaw->destroy();
    EXPECT_TRUE(behaviour->disableCalled);

    /// Scene processes its destroyqueue in update, so all GameObjects and their components inside the Q get deleted.
    scene.update(0.016f, *gameWorld.get());

    /// GO should now not exist anymore in the scene
    EXPECT_EQ(scene.getGameObject("ToDestroy"), nullptr);
}


