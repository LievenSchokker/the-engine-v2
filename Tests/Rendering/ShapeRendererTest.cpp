#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Rendering/Color.h"
#include "Rendering/IRenderer.h"
#include "Rendering/RenderCommand.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "Scene/Scene.h"

#include "Scene/SceneManager.h"
#include <gtest/gtest.h>
#include <memory>

struct FakeRenderer : public IRenderer
{
	void open(const WindowOptions&) override {}
	void close() override { isWindowOpen = false; }
	bool isOpen() override { return isWindowOpen; }
	void setTitle(const std::string&) override {}

    void beginFrame(const Color& color) override
    {
        lastClearColor = color;
        ++beginCalls;
    }

    void endFrame() override
    {
        ++endCalls;
    }

    void submitUI(const std::vector<UIRenderCommand>& command, InputManager& inputs) override
    {
    }

    void execute(const RenderCommand& command) override
    {
        executedCommands.push_back(command);

        switch (command.type)
        {
            case RenderCommandType::Circle:
                ++circleCalls;
                break;
            case RenderCommandType::Rectangle:
                ++rectangleCalls;
                break;
            default:
                break;
        }
    }

    void setUIRenderHook(std::unique_ptr<IUIRenderHook>) override
    {
    }
	void setupEvents(EventDispatcher&) override {}

	bool isWindowOpen = true;
    int beginCalls = 0;
    int endCalls = 0;
    int circleCalls = 0;
    int rectangleCalls = 0;
    Color lastClearColor = Color::black();
    std::vector<RenderCommand> executedCommands;
};


// Test that ShapeRenderer fills queue with circle command
TEST(ShapeRendererTest, FillsQueueWithCircleCommand)
{
    // Arrange
    auto gameworld = std::make_unique<GameWorld>();
    Scene scene("TestScene");
    scene.onStart(*gameworld);

    auto circle = std::make_unique<GameObject>();
    circle->getTransform()->setPosition({42.0, 24.0});
    circle->getTransform()->setScale({1.0, 1.0});
    circle->addComponent<ShapeRenderer>()->setCircle(25.0).setColor(Color::blue());
    scene.addGameObject(std::move(circle));

    // Act
    RenderQueue queue;
    for (auto* comp : scene.getAllComponentsOfType<RenderComponent>())
    {
        comp->fillRenderQueue(queue);
    }

    // Assert
    const auto& commands = queue.world().getCommands();
    ASSERT_EQ(commands.size(), 1);
    EXPECT_EQ(commands[0].type, RenderCommandType::Circle);
    EXPECT_DOUBLE_EQ(commands[0].radius, 25.0);
    EXPECT_DOUBLE_EQ(commands[0].position.x, 42.0);
    EXPECT_DOUBLE_EQ(commands[0].position.y, 24.0);
    EXPECT_EQ(commands[0].color.b, Color::blue().b);
}


// Test that ShapeRenderer fills queue with rectangle command
TEST(ShapeRendererTest, FillsQueueWithRectangleCommand)
{
    // Arrange
    auto gameworld = std::make_unique<GameWorld>();
    Scene scene("TestScene");
    scene.onStart(*gameworld);

    auto rect = std::make_unique<GameObject>();
    rect->getTransform()->setPosition({10.0, 12.0});
    rect->getTransform()->setRotationAngle(33.0);
    rect->getTransform()->setScale({1.0, 1.0});
    rect->addComponent<ShapeRenderer>()->setRectangle({80.0, 40.0}).setColor(Color::yellow());
    scene.addGameObject(std::move(rect));

    // Act
    RenderQueue queue;
    for (auto* comp : scene.getAllComponentsOfType<RenderComponent>())
    {
        comp->fillRenderQueue(queue);
    }

    // Assert
    const auto& commands = queue.world().getCommands();
    ASSERT_EQ(commands.size(), 1);
    EXPECT_EQ(commands[0].type, RenderCommandType::Rectangle);
    EXPECT_DOUBLE_EQ(commands[0].size.x, 80.0);
    EXPECT_DOUBLE_EQ(commands[0].size.y, 40.0);
    EXPECT_DOUBLE_EQ(commands[0].rotationDegrees, 33.0);
    EXPECT_EQ(commands[0].color.g, Color::yellow().g);
}

// Test RenderSystem executes commands through renderer
TEST(RenderSystemTest, ExecutesCommandsThroughRenderer)
{
    // Arrange
    auto gameworld = std::make_unique<GameWorld>();
    auto fakeRenderer = std::make_unique<FakeRenderer>();
    FakeRenderer* rendererPtr = fakeRenderer.get();
    RenderSystem renderSystem{std::move(fakeRenderer)};

    std::unique_ptr<Scene> scene = std::make_unique<Scene>("TestScene");
    scene->onStart(*gameworld);

    auto circle1 = std::make_unique<GameObject>();
    circle1->getTransform()->setPosition({42.0, 24.0});
    circle1->setLayer(2);
    auto* shape1 = circle1->addComponent<ShapeRenderer>();
    shape1->setCircle(25.0).setColor(Color::blue());
    scene->addGameObject(std::move(circle1));

    auto circle2 = std::make_unique<GameObject>();
    circle2->getTransform()->setPosition({10.0, 10.0});
    circle2->setLayer(1);
    auto* shape2 = circle2->addComponent<ShapeRenderer>();
    shape2->setCircle(15.0).setColor(Color::red());
    scene->addGameObject(std::move(circle2));

	auto world = std::make_unique<GameWorld>();
    auto sceneManager = std::make_unique<SceneManager>(*world);
    gameworld->sceneManager = sceneManager.get();
    sceneManager->addScene(std::move(scene));
    sceneManager->setActiveScene("TestScene");

    // Act
    renderSystem.update(0.016, *gameworld);

    // Assert - Layer 1 should render before Layer 2
    ASSERT_EQ(rendererPtr->executedCommands.size(), 2);
    EXPECT_EQ(rendererPtr->executedCommands[0].layer, 1);
    EXPECT_EQ(rendererPtr->executedCommands[1].layer, 2);
}

// Test inactive GameObjects are not rendered
TEST(RenderSystemTest, SkipsInactiveGameObjects)
{
    // Arrange
    auto gameworld = std::make_unique<GameWorld>();
    auto fakeRenderer = std::make_unique<FakeRenderer>();
    FakeRenderer* rendererPtr = fakeRenderer.get();
    RenderSystem renderSystem{std::move(fakeRenderer)};

    std::unique_ptr<Scene> scene = std::make_unique<Scene>("TestScene");
    scene->onStart(*gameworld);

    auto activeObj = std::make_unique<GameObject>();
    activeObj->addComponent<ShapeRenderer>()->setCircle(10.0);
    scene->addGameObject(std::move(activeObj));

    auto inactiveObj = std::make_unique<GameObject>();
    inactiveObj->addComponent<ShapeRenderer>()->setCircle(20.0);
    inactiveObj->setActive(false);
    scene->addGameObject(std::move(inactiveObj));

	auto world = std::make_unique<GameWorld>();
    auto sceneManager = std::make_unique<SceneManager>(*world);
    gameworld->sceneManager = sceneManager.get();
    sceneManager->addScene(std::move(scene));
    sceneManager->setActiveScene("TestScene");

    // Act
    renderSystem.update(0.016, *gameworld);

    // Assert - only active object should be rendered
    EXPECT_EQ(rendererPtr->executedCommands.size(), 1);
}
