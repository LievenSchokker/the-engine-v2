#include "Game.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <gtest/gtest.h>
#include <memory>
#include <string>

class MockGameObject : public GameObject
{
public:
    explicit MockGameObject(const std::string& name) : GameObject()
    {
        setName(name);
    }
};

TEST(SceneManagerTest, MainFlowIntegration)
{
    // Arrange
    std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    SceneManager sceneManager{*gameWorld};  // Was missing
    auto prototypeScene = std::make_unique<Scene>("PrototypeScene");
    auto gameObject = std::make_unique<MockGameObject>("ExampleGameObject");
    auto* gameObjectPtr = gameObject.get();
    prototypeScene->addRunTimeGameObject(std::move(gameObject), *gameWorld);
    bool sceneAdded = sceneManager.addScene(std::move(prototypeScene));
    ASSERT_TRUE(sceneAdded);

    // Act
    sceneManager.setActiveScene("PrototypeScene");

    // Assert
    Scene* scenePtr = sceneManager.getScene("PrototypeScene");
    ASSERT_NE(scenePtr, nullptr);
    EXPECT_EQ(scenePtr->getName(), "PrototypeScene");
    ASSERT_NE(sceneManager.getActiveScene(), nullptr);
    EXPECT_EQ(sceneManager.getActiveScene()->getName(), "PrototypeScene");
}

TEST(SceneManagerTest, CompleteLifecycle)
{
    // Arrange
    std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
	std::unique_ptr<Scene> scene = std::make_unique<Scene>("Scene");
	SceneManager sceneManager{*gameWorld};
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Object1");
    auto* obj1Ptr = obj1.get();
    scene1->addRunTimeGameObject(std::move(obj1), *gameWorld);

    auto scene2 = std::make_unique<Scene>("Scene2");
    auto obj2 = std::make_unique<MockGameObject>("Object2");
    auto* obj2Ptr = obj2.get();
    scene2->addRunTimeGameObject(std::move(obj2), *gameWorld);
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
    sceneManager.update(0.1f, gameWorld.get());

    // Assert
    EXPECT_EQ(sceneManager.getActiveScene()->getName(), "Scene1");

    // Act - Pause
    sceneManager.pause();
    sceneManager.update(0.1f, gameWorld.get());

    // Assert
    EXPECT_TRUE(sceneManager.isPaused());

    // Act - Pause again (should be no-op)
    sceneManager.pause();

    // Assert
    EXPECT_TRUE(sceneManager.isPaused());

    // Act - Resume
    sceneManager.resume();
    sceneManager.update(0.1f, gameWorld.get());

    // Assert
    EXPECT_FALSE(sceneManager.isPaused());

    // Act - Resume again (should be no-op)
    sceneManager.resume();

    // Assert
    EXPECT_FALSE(sceneManager.isPaused());

    // Act - Switch to Scene2
    sceneManager.setActiveScene("Scene2");
    sceneManager.update(0.2f, gameWorld.get());

    // Assert
    EXPECT_EQ(sceneManager.getActiveScene()->getName(), "Scene2");
    EXPECT_FALSE(sceneManager.isPaused());

    // Act - Remove active scene and try removing non-existent scene
    bool removed = sceneManager.removeScene("Scene2");
    bool removedNonExistent = sceneManager.removeScene("NonExistent");

    // Assert
    EXPECT_TRUE(removed);
    EXPECT_EQ(sceneManager.getActiveScene(), nullptr);
    EXPECT_FALSE(sceneManager.isPaused());
    EXPECT_FALSE(removedNonExistent);
}

TEST(SceneManagerTest, ErrorHandling)
{
    // Arrange
    std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    SceneManager sceneManager{*gameWorld};
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

TEST(SceneManagerTest, AddGameObjectToActiveScene)
{
    // Arrange
    std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    SceneManager sceneManager{*gameWorld};
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Object1");
    scene1->addRunTimeGameObject(std::move(obj1), *gameWorld);
    bool sceneAdded = sceneManager.addScene(std::move(scene1));
    ASSERT_TRUE(sceneAdded);
    Scene* scenePtr = sceneManager.getScene("Scene1");
    ASSERT_NE(scenePtr, nullptr);

    // Act & Assert - Add object to inactive scene
    auto obj2 = std::make_unique<MockGameObject>("Object2");
    auto* obj2Ptr = obj2.get();
    scenePtr->addRunTimeGameObject(std::move(obj2), *gameWorld);

    // Act - Activate scene
    sceneManager.setActiveScene("Scene1");

    // Assert - Objects added before activation should exist
    EXPECT_NE(scenePtr->getGameObject("Object1"), nullptr);
    EXPECT_NE(scenePtr->getGameObject("Object2"), nullptr);

    // Act - Add object to active scene
    auto obj3 = std::make_unique<MockGameObject>("Object3");
    auto* obj3Ptr = obj3.get();
    scenePtr->addRunTimeGameObject(std::move(obj3), *gameWorld);

    // Assert - Object added to active scene should exist
    EXPECT_NE(scenePtr->getGameObject("Object3"), nullptr);
}

TEST(SceneManagerTest, UpdateWithNoActiveScene)
{
    // Arrange
    std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    SceneManager sceneManager{*gameWorld};
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Object1");
    scene1->addRunTimeGameObject(std::move(obj1), *gameWorld);
    sceneManager.addScene(std::move(scene1));

    // Act - Should not crash with no active scene
    sceneManager.update(0.1f, gameWorld.get());

    // Assert - Scene should still be retrievable
    EXPECT_NE(sceneManager.getScene("Scene1"), nullptr);
    EXPECT_EQ(sceneManager.getActiveScene(), nullptr);
}

TEST(SceneManagerTest, TransferGameObject)
{
    // Arrange
    std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    SceneManager sceneManager{*gameWorld};
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Player");
    auto* obj1Ptr = obj1.get();
    scene1->addRunTimeGameObject(std::move(obj1), *gameWorld);

    auto scene2 = std::make_unique<Scene>("Scene2");
    sceneManager.addScene(std::move(scene1));
    sceneManager.addScene(std::move(scene2));

    // Act & Assert - Transfer while both scenes are inactive
    EXPECT_TRUE(sceneManager.transferGameObject("Scene1", "Scene2", "Player"));

    Scene* scene2Ptr = sceneManager.getScene("Scene2");
    ASSERT_NE(scene2Ptr, nullptr);
    EXPECT_NE(scene2Ptr->getGameObject("Player"), nullptr);
    EXPECT_EQ(sceneManager.getScene("Scene1")->getGameObject("Player"), nullptr);

    // Act - Activate Scene2
    sceneManager.setActiveScene("Scene2");

    // Assert - Object should be in active scene
    EXPECT_NE(scene2Ptr->getGameObject("Player"), nullptr);

    // Arrange - Add Scene3 with new object
    auto scene3 = std::make_unique<Scene>("Scene3");
    auto obj2 = std::make_unique<MockGameObject>("Enemy");
    auto* obj2Ptr = obj2.get();
    scene3->addRunTimeGameObject(std::move(obj2), *gameWorld);
    sceneManager.addScene(std::move(scene3));
    sceneManager.setActiveScene("Scene3");

    // Act - Transfer from active scene
    EXPECT_TRUE(sceneManager.transferGameObject("Scene3", "Scene2", "Enemy"));

    // Assert - Object should now be in Scene2
    EXPECT_EQ(scene2Ptr->getGameObject("Enemy"), obj2Ptr);
    EXPECT_EQ(sceneManager.getScene("Scene3")->getGameObject("Enemy"), nullptr);
}

TEST(SceneManagerTest, TransferGameObjectErrorCases)
{
    // Arrange
    std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    SceneManager sceneManager{*gameWorld};
    auto scene1 = std::make_unique<Scene>("Scene1");
    auto obj1 = std::make_unique<MockGameObject>("Player");
    scene1->addRunTimeGameObject(std::move(obj1), *gameWorld);

    auto scene2 = std::make_unique<Scene>("Scene2");
    auto obj2 = std::make_unique<MockGameObject>("Player");
    scene2->addRunTimeGameObject(std::move(obj2), *gameWorld);

    sceneManager.addScene(std::move(scene1));
    sceneManager.addScene(std::move(scene2));

    // Act & Assert
    EXPECT_FALSE(sceneManager.transferGameObject("Scene1", "NonExistent", "Player"));
    EXPECT_FALSE(sceneManager.transferGameObject("NonExistent", "Scene2", "Player"));
    EXPECT_FALSE(sceneManager.transferGameObject("Scene1", "Scene2", "NonExistent"));
    EXPECT_FALSE(sceneManager.transferGameObject("Scene1", "Scene2", "Player"));
}

TEST(SceneTest, ExtractGameObject)
{
    // Arrange
    std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>();
    Scene scene("TestScene");
    auto obj = std::make_unique<MockGameObject>("TestObj");
    auto* objPtr = obj.get();
    scene.addRunTimeGameObject(std::move(obj), *gameWorld);

    // Act & Assert - Extract from inactive scene
    auto extracted = scene.extractGameObject("TestObj");
    ASSERT_NE(extracted, nullptr);
    EXPECT_EQ(extracted.get(), objPtr);
    EXPECT_EQ(scene.getGameObject("TestObj"), nullptr);

    // Arrange - Extract from active scene
    Scene scene2("TestScene2");
    auto obj2 = std::make_unique<MockGameObject>("TestObj2");
    auto* obj2Ptr = obj2.get();
    scene2.addRunTimeGameObject(std::move(obj2), *gameWorld);
    scene2.onStart(*gameWorld);

    // Act & Assert
    auto extracted2 = scene2.extractGameObject("TestObj2");
    ASSERT_NE(extracted2, nullptr);
    EXPECT_EQ(extracted2.get(), obj2Ptr);
    EXPECT_EQ(scene2.getGameObject("TestObj2"), nullptr);

    // Act & Assert - Extract non-existent object
    auto extracted3 = scene2.extractGameObject("NonExistent");
    EXPECT_EQ(extracted3, nullptr);
}