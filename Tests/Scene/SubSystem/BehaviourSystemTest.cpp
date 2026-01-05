#include <gtest/gtest.h>
#include "Scene/SubSystems/BehaviourSystem.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Core/GameWorld.h"
#include "../../Behaviour/TestBehaviours.h"

namespace engine_tests
{
class TestBehaviourTwo;
}

class BehaviourSystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        gameWorld = std::make_unique<GameWorld>();
        behaviourSystem = std::make_unique<BehaviourSystem>();
    }

    std::unique_ptr<GameWorld> gameWorld;
    std::unique_ptr<BehaviourSystem> behaviourSystem;
};

TEST_F(BehaviourSystemTest, InitialiseSceneCallsLifecycleMethods)
{
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();
    scene.addGameObject(std::move(go));

    // Before initialisation
    EXPECT_FALSE(behaviour->awakeCalled);
    EXPECT_FALSE(behaviour->enableCalled);
    EXPECT_FALSE(behaviour->startCalled);

    // Initialise
    behaviourSystem->initialiseScene(scene, *gameWorld);

    // After initialisation
    EXPECT_TRUE(behaviour->awakeCalled);
    EXPECT_TRUE(behaviour->enableCalled);
    EXPECT_TRUE(behaviour->startCalled);
}

TEST_F(BehaviourSystemTest, UpdateCallsBehaviourUpdate)
{
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();
    scene.addGameObject(std::move(go));

    behaviourSystem->initialiseScene(scene, *gameWorld);
    behaviour->updateCalled = false;  // Reset after initialisation

    behaviourSystem->update(scene, 0.016, *gameWorld);

    EXPECT_TRUE(behaviour->updateCalled);
}

TEST_F(BehaviourSystemTest, UpdateSkipsInactiveGameObjects)
{
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();
    scene.addGameObject(std::move(go));

    behaviourSystem->initialiseScene(scene, *gameWorld);
    behaviour->updateCalled = false;

    // Deactivate GameObject
    behaviour->getGameObject()->setActive(false);

    behaviourSystem->update(scene, 0.016, *gameWorld);

    EXPECT_FALSE(behaviour->updateCalled);
}

TEST_F(BehaviourSystemTest, UpdateSkipsDisabledBehaviours)
{
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();
    scene.addGameObject(std::move(go));

    behaviourSystem->initialiseScene(scene, *gameWorld);
    behaviour->updateCalled = false;

    // Disable behaviour
    behaviour->setEnabled(false);

    behaviourSystem->update(scene, 0.016, *gameWorld);

    EXPECT_FALSE(behaviour->updateCalled);
}

TEST_F(BehaviourSystemTest, InitialiseRuntimeBehavioursForNewGameObject)
{
    Scene scene{"TestScene"};

    // Start scene with no objects
    behaviourSystem->initialiseScene(scene, *gameWorld);

    // Add GameObject at runtime
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();

    EXPECT_FALSE(behaviour->awakeCalled);
    EXPECT_FALSE(behaviour->enableCalled);
    EXPECT_FALSE(behaviour->startCalled);

    std::vector<Behaviour*> behaviours = go->getAllBehaviours();
    scene.addGameObject(std::move(go));

    // Manually initialise runtime behaviours
    behaviourSystem->initialiseRuntimeBehaviours(behaviours, *gameWorld);

    EXPECT_TRUE(behaviour->awakeCalled);
    EXPECT_TRUE(behaviour->enableCalled);
    EXPECT_TRUE(behaviour->startCalled);
}

TEST_F(BehaviourSystemTest, DisableBehavioursCallsOnDisable)
{
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();
    std::vector<Behaviour*> behaviours = go->getAllBehaviours();

    behaviourSystem->awakeBehaviours(behaviours, *gameWorld);
    behaviourSystem->enableBehaviours(behaviours);

    EXPECT_FALSE(behaviour->disableCalled);

    behaviourSystem->disableBehaviours(behaviours);

    EXPECT_TRUE(behaviour->disableCalled);
}

TEST_F(BehaviourSystemTest, EnableBehavioursSkipsInactiveGameObjects)
{
    auto go = std::make_unique<GameObject>();
    auto* behaviour = go->addComponent<engine_tests::TestBehaviourOne>();
    go->setActive(false);  // Deactivate
    std::vector<Behaviour*> behaviours = go->getAllBehaviours();

    behaviourSystem->awakeBehaviours(behaviours, *gameWorld);
    behaviourSystem->enableBehaviours(behaviours);

    EXPECT_FALSE(behaviour->enableCalled);
}

TEST_F(BehaviourSystemTest, MultipleBehavioursInitialised)
{
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    auto* behaviour1 = go->addComponent<engine_tests::TestBehaviourOne>();
    auto* behaviour2 = go->addComponent<engine_tests::TestBehaviourTwo>();
    auto* behaviour3 = go->addComponent<engine_tests::TestBehaviourThree>();
    scene.addGameObject(std::move(go));

    behaviourSystem->initialiseScene(scene, *gameWorld);

    // All should be initialised
    EXPECT_TRUE(behaviour1->awakeCalled);
    EXPECT_TRUE(behaviour1->enableCalled);
    EXPECT_TRUE(behaviour1->startCalled);

    EXPECT_TRUE(behaviour2->awakeCalled);
    EXPECT_TRUE(behaviour2->enableCalled);
    EXPECT_TRUE(behaviour2->startCalled);

    EXPECT_TRUE(behaviour3->awakeCalled);
    EXPECT_TRUE(behaviour3->enableCalled);
    EXPECT_TRUE(behaviour3->startCalled);
}

TEST_F(BehaviourSystemTest, MultipleGameObjectsInitialised)
{
    Scene scene{"TestScene"};

    auto go1 = std::make_unique<GameObject>();
    auto* behaviour1 = go1->addComponent<engine_tests::TestBehaviourOne>();

    auto go2 = std::make_unique<GameObject>();
    auto* behaviour2 = go2->addComponent<engine_tests::TestBehaviourTwo>();

    scene.addGameObject(std::move(go1));
    scene.addGameObject(std::move(go2));

    behaviourSystem->initialiseScene(scene, *gameWorld);

    EXPECT_TRUE(behaviour1->awakeCalled);
    EXPECT_TRUE(behaviour1->startCalled);
    EXPECT_TRUE(behaviour2->awakeCalled);
    EXPECT_TRUE(behaviour2->startCalled);
}

TEST_F(BehaviourSystemTest, UpdateMultipleGameObjects)
{
    Scene scene{"TestScene"};

    auto go1 = std::make_unique<GameObject>();
    auto* behaviour1 = go1->addComponent<engine_tests::TestBehaviourOne>();

    auto go2 = std::make_unique<GameObject>();
    auto* behaviour2 = go2->addComponent<engine_tests::TestBehaviourTwo>();

    scene.addGameObject(std::move(go1));
    scene.addGameObject(std::move(go2));

    behaviourSystem->initialiseScene(scene, *gameWorld);
    behaviour1->updateCalled = false;
    behaviour2->updateCalled = false;

    behaviourSystem->update(scene, 0.016, *gameWorld);

    EXPECT_TRUE(behaviour1->updateCalled);
    EXPECT_TRUE(behaviour2->updateCalled);
}