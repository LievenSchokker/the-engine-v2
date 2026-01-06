#include <gtest/gtest.h>
#include "Scene/SubSystems/DestroySystem.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"

class DestroySystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        destroySystem = std::make_unique<DestroySystem>();
    }

    std::unique_ptr<DestroySystem> destroySystem;
};

TEST_F(DestroySystemTest, QueueDestroy)
{
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    go->setName("TestObject");
    GameObject* goPtr = go.get();
    scene.addGameObject(std::move(go));

    EXPECT_FALSE(destroySystem->isQueued(goPtr));
    EXPECT_EQ(destroySystem->queueSize(), 0);

    destroySystem->queueDestroy(goPtr);

    EXPECT_TRUE(destroySystem->isQueued(goPtr));
    EXPECT_EQ(destroySystem->queueSize(), 1);
}

TEST_F(DestroySystemTest, QueueDestroyIgnoresNull)
{
    destroySystem->queueDestroy(nullptr);
    EXPECT_EQ(destroySystem->queueSize(), 0);
}

TEST_F(DestroySystemTest, QueueDestroyIgnoresDuplicates)
{
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    GameObject* goPtr = go.get();
    scene.addGameObject(std::move(go));

    destroySystem->queueDestroy(goPtr);
    destroySystem->queueDestroy(goPtr);
    destroySystem->queueDestroy(goPtr);

    EXPECT_EQ(destroySystem->queueSize(), 1);
}

TEST_F(DestroySystemTest, ProcessQueueRemovesFromScene)
{
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    go->setName("TestObject");
    GameObject* goPtr = go.get();
    scene.addGameObject(std::move(go));

    EXPECT_NE(scene.getGameObject("TestObject"), nullptr);

    destroySystem->queueDestroy(goPtr);
    destroySystem->processQueue(scene);

    EXPECT_EQ(scene.getGameObject("TestObject"), nullptr);
    EXPECT_EQ(destroySystem->queueSize(), 0);
}

TEST_F(DestroySystemTest, ClearEmptiesQueue)
{
    Scene scene{"TestScene"};
    auto go1 = std::make_unique<GameObject>();
    auto go2 = std::make_unique<GameObject>();
    GameObject* go1Ptr = go1.get();
    GameObject* go2Ptr = go2.get();
    scene.addGameObject(std::move(go1));
    scene.addGameObject(std::move(go2));

    destroySystem->queueDestroy(go1Ptr);
    destroySystem->queueDestroy(go2Ptr);
    EXPECT_EQ(destroySystem->queueSize(), 2);

    destroySystem->clear();
    EXPECT_EQ(destroySystem->queueSize(), 0);
}

TEST_F(DestroySystemTest, PreDestroyCallbackInvoked)
{
    Scene scene{"TestScene"};
    auto go = std::make_unique<GameObject>();
    go->setName("TestObject");
    GameObject* goPtr = go.get();
    scene.addGameObject(std::move(go));

    bool callbackInvoked = false;
    GameObject* callbackObject = nullptr;

    destroySystem->setPreDestroyCallback([&](GameObject* obj) {
        callbackInvoked = true;
        callbackObject = obj;
    });

    destroySystem->queueDestroy(goPtr);
    destroySystem->processQueue(scene);

    EXPECT_TRUE(callbackInvoked);
    EXPECT_EQ(callbackObject, goPtr);
}