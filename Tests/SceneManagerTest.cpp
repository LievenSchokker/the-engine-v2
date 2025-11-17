#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "../include/GameObject.h"
#include "../include/Scene.h"
#include "../include/SceneManager.h"

class MockGameObject : public GameObject {
   public:
    explicit MockGameObject(std::string name) : GameObject(std::move(name)) {
    }

    int onStartCallCount = 0;
    int onStopCallCount = 0;
    int onPauseCallCount = 0;
    int onResumeCallCount = 0;
    int updateCallCount = 0;
    int renderCallCount = 0;
    float totalDeltaTime = 0.0f;

    void onStart() override {
        onStartCallCount++;
    }

    void onStop() override {
        onStopCallCount++;
    }

    void onPause() override {
        onPauseCallCount++;
    }

    void onResume() override {
        onResumeCallCount++;
    }

    void update(float deltaTime) override {
        updateCallCount++;
        totalDeltaTime += deltaTime;
    }
};

TEST(SceneManagerTest, MainFlowIntegration) {
    // Arrange
    SceneManager sceneManager;
    auto prototypeScene = std::make_unique<Scene>("PrototypeScene");
    auto gameObject = std::make_unique<MockGameObject>("ExampleGameObject");
    auto* gameObjectPtr = gameObject.get();
    prototypeScene->addGameObject(std::move(gameObject));
    bool sceneAdded = sceneManager.addScene(std::move(prototypeScene));
    ASSERT_TRUE(sceneAdded);
    Scene* scenePtr = sceneManager.getScene("PrototypeScene");
    ASSERT_NE(scenePtr, nullptr);

    // Act
    sceneManager.setActiveScene("PrototypeScene");
    const float deltaTime1 = 0.016f;
    const float deltaTime2 = 0.033f;
    sceneManager.update(deltaTime1);
    sceneManager.render();
    sceneManager.update(deltaTime2);
    sceneManager.render();

    // Assert
    ASSERT_NE(scenePtr, nullptr);
    EXPECT_EQ(scenePtr->getName(), "PrototypeScene");
    ASSERT_NE(sceneManager.getActiveScene(), nullptr);
    EXPECT_EQ(sceneManager.getActiveScene()->getName(), "PrototypeScene");
    EXPECT_EQ(gameObjectPtr->onStartCallCount, 1);
    EXPECT_EQ(gameObjectPtr->onStopCallCount, 0);
    EXPECT_EQ(gameObjectPtr->updateCallCount, 2);
    EXPECT_EQ(gameObjectPtr->renderCallCount, 2);
    EXPECT_FLOAT_EQ(gameObjectPtr->totalDeltaTime, deltaTime1 + deltaTime2);
}

TEST(SceneManagerTest, CompleteLifecycle) {
    // Arrange
    SceneManager sceneManager;
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Object1");
    auto* obj1Ptr = obj1.get();
    scene1->addGameObject(std::move(obj1));

    auto scene2 = std::make_unique<Scene>("Scene2");
    auto obj2 = std::make_unique<MockGameObject>("Object2");
    auto* obj2Ptr = obj2.get();
    scene2->addGameObject(std::move(obj2));
    sceneManager.addScene(std::move(scene1));
    sceneManager.addScene(std::move(scene2));

    // Act & Assert - Initial state
    EXPECT_EQ(sceneManager.getActiveScene(), nullptr);
    EXPECT_FALSE(sceneManager.isPaused());

    // Act & Assert - Pause/resume with no active scene
    sceneManager.pause();
    EXPECT_FALSE(sceneManager.isPaused());
    sceneManager.resume();
    EXPECT_FALSE(sceneManager.isPaused());

    // Act - Activate Scene1
    sceneManager.loadScene("Scene1");
    sceneManager.update(0.1f);
    sceneManager.render();

    // Assert
    EXPECT_EQ(sceneManager.getActiveScene()->getName(), "Scene1");
    EXPECT_EQ(obj1Ptr->onStartCallCount, 1);
    EXPECT_EQ(obj1Ptr->updateCallCount, 1);
    EXPECT_EQ(obj1Ptr->renderCallCount, 1);
    EXPECT_EQ(obj2Ptr->onStartCallCount, 0);
    EXPECT_EQ(obj2Ptr->updateCallCount, 0);
    EXPECT_EQ(obj2Ptr->renderCallCount, 0);

    // Act - Pause
    sceneManager.pause();
    sceneManager.update(0.1f);
    sceneManager.render();

    // Assert
    EXPECT_TRUE(sceneManager.isPaused());
    EXPECT_EQ(obj1Ptr->onPauseCallCount, 1);
    EXPECT_EQ(obj1Ptr->updateCallCount, 1);
    EXPECT_EQ(obj1Ptr->renderCallCount, 1);

    // Act - Pause again (should be no-op)
    sceneManager.pause();

    // Assert
    EXPECT_TRUE(sceneManager.isPaused());
    EXPECT_EQ(obj1Ptr->onPauseCallCount, 1);

    // Act - Resume
    sceneManager.resume();
    sceneManager.update(0.1f);
    sceneManager.render();

    // Assert
    EXPECT_FALSE(sceneManager.isPaused());
    EXPECT_EQ(obj1Ptr->onResumeCallCount, 1);
    EXPECT_EQ(obj1Ptr->updateCallCount, 2);
    EXPECT_EQ(obj1Ptr->renderCallCount, 2);

    // Act - Resume again (should be no-op)
    sceneManager.resume();

    // Assert
    EXPECT_FALSE(sceneManager.isPaused());
    EXPECT_EQ(obj1Ptr->onResumeCallCount, 1);

    // Act - Switch to Scene2
    sceneManager.setActiveScene("Scene2");
    sceneManager.update(0.2f);
    sceneManager.render();

    // Assert
    EXPECT_EQ(sceneManager.getActiveScene()->getName(), "Scene2");
    EXPECT_FALSE(sceneManager.isPaused());
    EXPECT_EQ(obj1Ptr->onStopCallCount, 1);
    EXPECT_EQ(obj1Ptr->updateCallCount, 2);
    EXPECT_EQ(obj2Ptr->onStartCallCount, 1);
    EXPECT_EQ(obj2Ptr->updateCallCount, 1);
    EXPECT_EQ(obj2Ptr->renderCallCount, 1);

    // Act - Remove active scene and try removing non-existent scene
    bool removed = sceneManager.removeScene("Scene2");
    bool removedNonExistent = sceneManager.removeScene("NonExistent");

    // Assert
    EXPECT_TRUE(removed);
    EXPECT_EQ(sceneManager.getActiveScene(), nullptr);
    EXPECT_FALSE(sceneManager.isPaused());
    EXPECT_FALSE(removedNonExistent);
}

TEST(SceneManagerTest, ErrorHandling) {
    // Arrange
    SceneManager sceneManager;
    auto scene1 = std::make_unique<Scene>("ValidScene");
    sceneManager.addScene(std::move(scene1));

    // Act & Assert - Null scene
    bool result = sceneManager.addScene(nullptr);
    EXPECT_FALSE(result);

    // Act & Assert - Duplicate scene name
    auto duplicateScene = std::make_unique<Scene>("ValidScene");
    result = sceneManager.addScene(std::move(duplicateScene));
    EXPECT_FALSE(result);

    // Act & Assert - Get non-existent scene
    Scene* notFound = sceneManager.getScene("NonExistent");
    EXPECT_EQ(notFound, nullptr);

    // Act & Assert - Set active scene that doesn't exist
    EXPECT_FALSE(sceneManager.setActiveScene("NonExistent"));
    EXPECT_EQ(sceneManager.getActiveScene(), nullptr);
}

TEST(SceneManagerTest, AddGameObjectToActiveScene) {
    // Arrange
    SceneManager sceneManager;
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Object1");
    auto* obj1Ptr = obj1.get();
    scene1->addGameObject(std::move(obj1));
    bool sceneAdded = sceneManager.addScene(std::move(scene1));
    ASSERT_TRUE(sceneAdded);
    Scene* scenePtr = sceneManager.getScene("Scene1");
    ASSERT_NE(scenePtr, nullptr);

    // Act & Assert - Add object to inactive scene
    auto obj2 = std::make_unique<MockGameObject>("Object2");
    auto* obj2Ptr = obj2.get();
    scenePtr->addGameObject(std::move(obj2));
    EXPECT_EQ(obj1Ptr->onStartCallCount, 0);
    EXPECT_EQ(obj2Ptr->onStartCallCount, 0);

    // Act - Activate scene
    sceneManager.setActiveScene("Scene1");

    // Assert - Objects added before activation should start
    EXPECT_EQ(obj1Ptr->onStartCallCount, 1);
    EXPECT_EQ(obj2Ptr->onStartCallCount, 1);

    // Act - Add object to active scene
    auto obj3 = std::make_unique<MockGameObject>("Object3");
    auto* obj3Ptr = obj3.get();
    scenePtr->addGameObject(std::move(obj3));

    // Assert - Object added to active scene should start immediately
    EXPECT_EQ(obj3Ptr->onStartCallCount, 1);
}

TEST(SceneManagerTest, UpdateRenderWithNoActiveScene) {
    // Arrange
    SceneManager sceneManager;
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Object1");
    auto* obj1Ptr = obj1.get();
    scene1->addGameObject(std::move(obj1));
    sceneManager.addScene(std::move(scene1));

    // Act
    sceneManager.update(0.1f);
    sceneManager.render();

    // Assert
    EXPECT_EQ(obj1Ptr->updateCallCount, 0);
    EXPECT_EQ(obj1Ptr->renderCallCount, 0);
}

TEST(SceneManagerTest, TransferGameObject) {
    // Arrange
    SceneManager sceneManager;
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Player");
    auto* obj1Ptr = obj1.get();
    scene1->addGameObject(std::move(obj1));

    auto scene2 = std::make_unique<Scene>("Scene2");
    sceneManager.addScene(std::move(scene1));
    sceneManager.addScene(std::move(scene2));

    // Act & Assert - Transfer while both scenes are inactive
    EXPECT_TRUE(sceneManager.transferGameObject("Scene1", "Scene2", "Player"));
    EXPECT_EQ(obj1Ptr->onStopCallCount, 0);
    EXPECT_EQ(obj1Ptr->onStartCallCount, 0);

    Scene* scene2Ptr = sceneManager.getScene("Scene2");
    ASSERT_NE(scene2Ptr, nullptr);
    EXPECT_NE(scene2Ptr->getGameObject("Player"), nullptr);
    EXPECT_EQ(sceneManager.getScene("Scene1")->getGameObject("Player"), nullptr);

    // Act - Activate Scene2
    sceneManager.setActiveScene("Scene2");

    // Assert
    EXPECT_EQ(obj1Ptr->onStartCallCount, 1);

    // Arrange - Activate Scene1 with new object
    auto scene3 = std::make_unique<Scene>("Scene3");
    auto obj2 = std::make_unique<MockGameObject>("Enemy");
    auto* obj2Ptr = obj2.get();
    scene3->addGameObject(std::move(obj2));
    sceneManager.addScene(std::move(scene3));
    sceneManager.setActiveScene("Scene3");
    EXPECT_EQ(obj2Ptr->onStartCallCount, 1);

    // Act - Transfer from active scene
    EXPECT_TRUE(sceneManager.transferGameObject("Scene3", "Scene2", "Enemy"));

    // Assert
    EXPECT_EQ(obj2Ptr->onStopCallCount, 1);
    EXPECT_EQ(scene2Ptr->getGameObject("Enemy"), obj2Ptr);
}

TEST(SceneManagerTest, TransferGameObjectErrorCases) {
    // Arrange
    SceneManager sceneManager;
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Player");
    scene1->addGameObject(std::move(obj1));

    auto scene2 = std::make_unique<Scene>("Scene2");
    auto obj2 = std::make_unique<MockGameObject>("Player");
    scene2->addGameObject(std::move(obj2));

    sceneManager.addScene(std::move(scene1));
    sceneManager.addScene(std::move(scene2));

    // Act & Assert
    EXPECT_FALSE(sceneManager.transferGameObject("Scene1", "NonExistent", "Player"));
    EXPECT_FALSE(sceneManager.transferGameObject("NonExistent", "Scene2", "Player"));
    EXPECT_FALSE(sceneManager.transferGameObject("Scene1", "Scene2", "NonExistent"));
    EXPECT_FALSE(sceneManager.transferGameObject("Scene1", "Scene2", "Player"));
}

TEST(SceneTest, ExtractGameObject) {
    // Arrange
    Scene scene("TestScene");
    auto obj = std::make_unique<MockGameObject>("TestObj");
    auto* objPtr = obj.get();
    scene.addGameObject(std::move(obj));

    // Act & Assert - Extract from inactive scene
    auto extracted = scene.extractGameObject("TestObj");
    ASSERT_NE(extracted, nullptr);
    EXPECT_EQ(extracted.get(), objPtr);
    EXPECT_EQ(scene.getGameObject("TestObj"), nullptr);
    EXPECT_EQ(objPtr->onStopCallCount, 0);

    // Arrange - Extract from active scene
    Scene scene2("TestScene2");
    auto obj2 = std::make_unique<MockGameObject>("TestObj2");
    auto* obj2Ptr = obj2.get();
    scene2.addGameObject(std::move(obj2));
    scene2.onStart();

    // Act & Assert
    auto extracted2 = scene2.extractGameObject("TestObj2");
    ASSERT_NE(extracted2, nullptr);
    EXPECT_EQ(extracted2.get(), obj2Ptr);
    EXPECT_EQ(scene2.getGameObject("TestObj2"), nullptr);
    EXPECT_EQ(obj2Ptr->onStopCallCount, 1);

    // Act & Assert - Extract non-existent object
    auto extracted3 = scene2.extractGameObject("NonExistent");
    EXPECT_EQ(extracted3, nullptr);
}
