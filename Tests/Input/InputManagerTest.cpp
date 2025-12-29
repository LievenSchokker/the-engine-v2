#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"
#include "Core/GameWorld.h"
#include "Events/EventDispatcher/EventDispatcher.h"
#include "Events/EventImplementations/ApplicationEvents.h"
#include <gtest/gtest.h>
#include <memory>

class InputManagerTest : public ::testing::Test
{
protected:
    std::unique_ptr<InputManager> input;
    std::unique_ptr<EventDispatcher> dispatcher;
    GameWorld gameWorld;

    void SetUp() override
    {
        input = std::make_unique<InputManager>();
        dispatcher = std::make_unique<EventDispatcher>();
        input->initialize(*dispatcher);
    }

    void TearDown() override
    {
        input.reset();
        dispatcher.reset();
    }

    void pressKey(KeyCode key)
    {
        KeyPressedEvent event(0, static_cast<int>(key), false);
        dispatcher->dispatch(event);
    }

    void releaseKey(KeyCode key)
    {
        KeyReleasedEvent event(0, static_cast<int>(key));
        dispatcher->dispatch(event);
    }

    void pressMouseButton(MouseButton button)
    {
        MouseButtonPressedEvent event(button, 0, 0, 1);
        dispatcher->dispatch(event);
    }

    void releaseMouseButton(MouseButton button)
    {
        MouseButtonReleasedEvent event(button, 0, 0);
        dispatcher->dispatch(event);
    }

    void moveMouse(int x, int y)
    {
        MouseMovedEvent event(x, y, 0, 0);
        dispatcher->dispatch(event);
    }

    void scrollMouse(float dx, float dy)
    {
        MouseScrollEvent event(dx, dy);
        dispatcher->dispatch(event);
    }

    void closeWindow()
    {
        WindowCloseEvent event;
        dispatcher->dispatch(event);
    }

    void nextFrame()
    {
        input->update(0.016, gameWorld);
    }
};

TEST_F(InputManagerTest, KeyPressAndRelease)
{
    pressKey(KeyCode::W);
    nextFrame();

    EXPECT_TRUE(input->wasKeyPressed(KeyCode::W));
    EXPECT_TRUE(input->isKeyDown(KeyCode::W));
    EXPECT_FALSE(input->wasKeyReleased(KeyCode::W));

    nextFrame();

    EXPECT_FALSE(input->wasKeyPressed(KeyCode::W));
    EXPECT_TRUE(input->isKeyDown(KeyCode::W));
    EXPECT_FALSE(input->wasKeyReleased(KeyCode::W));

    releaseKey(KeyCode::W);
    nextFrame();

    EXPECT_FALSE(input->wasKeyPressed(KeyCode::W));
    EXPECT_FALSE(input->isKeyDown(KeyCode::W));
    EXPECT_TRUE(input->wasKeyReleased(KeyCode::W));

    nextFrame();

    EXPECT_FALSE(input->wasKeyPressed(KeyCode::W));
    EXPECT_FALSE(input->isKeyDown(KeyCode::W));
    EXPECT_FALSE(input->wasKeyReleased(KeyCode::W));
}

TEST_F(InputManagerTest, MouseButtonPressAndRelease)
{
    pressMouseButton(MouseButton::LEFT);
    nextFrame();

    EXPECT_TRUE(input->wasMousePressed(MouseButton::LEFT));
    EXPECT_TRUE(input->isMouseDown(MouseButton::LEFT));
    EXPECT_FALSE(input->wasMouseReleased(MouseButton::LEFT));

    nextFrame();

    EXPECT_FALSE(input->wasMousePressed(MouseButton::LEFT));
    EXPECT_TRUE(input->isMouseDown(MouseButton::LEFT));

    releaseMouseButton(MouseButton::LEFT);
    nextFrame();

    EXPECT_TRUE(input->wasMouseReleased(MouseButton::LEFT));
    EXPECT_FALSE(input->isMouseDown(MouseButton::LEFT));
}

TEST_F(InputManagerTest, MultipleSimultaneousInputs)
{
    pressKey(KeyCode::W);
    pressKey(KeyCode::A);
    pressMouseButton(MouseButton::LEFT);
    moveMouse(100, 200);
    nextFrame();

    EXPECT_TRUE(input->isKeyDown(KeyCode::W));
    EXPECT_TRUE(input->isKeyDown(KeyCode::A));
    EXPECT_TRUE(input->wasKeyPressed(KeyCode::W));
    EXPECT_TRUE(input->wasKeyPressed(KeyCode::A));
    EXPECT_TRUE(input->isMouseDown(MouseButton::LEFT));
    EXPECT_EQ(input->mouseX(), 100);
    EXPECT_EQ(input->mouseY(), 200);

    pressKey(KeyCode::D);
    pressMouseButton(MouseButton::RIGHT);
    nextFrame();

    EXPECT_TRUE(input->isKeyDown(KeyCode::W));
    EXPECT_TRUE(input->isKeyDown(KeyCode::A));
    EXPECT_TRUE(input->isKeyDown(KeyCode::D));
    EXPECT_TRUE(input->wasKeyPressed(KeyCode::D));
    EXPECT_FALSE(input->wasKeyPressed(KeyCode::W));
    EXPECT_TRUE(input->wasMousePressed(MouseButton::RIGHT));

    releaseKey(KeyCode::W);
    releaseMouseButton(MouseButton::LEFT);
    nextFrame();

    EXPECT_FALSE(input->isKeyDown(KeyCode::W));
    EXPECT_TRUE(input->wasKeyReleased(KeyCode::W));
    EXPECT_TRUE(input->isKeyDown(KeyCode::A));
    EXPECT_TRUE(input->isKeyDown(KeyCode::D));
    EXPECT_FALSE(input->isMouseDown(MouseButton::LEFT));
    EXPECT_TRUE(input->wasMouseReleased(MouseButton::LEFT));
}

TEST_F(InputManagerTest, WindowCloseSignalsQuit)
{
    EXPECT_FALSE(input->quitRequested());

    closeWindow();

    EXPECT_TRUE(input->quitRequested());
}

TEST_F(InputManagerTest, KeyRepeatIsIgnored)
{
    pressKey(KeyCode::W);
    nextFrame();

    EXPECT_TRUE(input->wasKeyPressed(KeyCode::W));
    EXPECT_TRUE(input->isKeyDown(KeyCode::W));

    KeyPressedEvent repeatEvent(0, static_cast<int>(KeyCode::W), true);
    dispatcher->dispatch(repeatEvent);
    nextFrame();

    EXPECT_FALSE(input->wasKeyPressed(KeyCode::W));
    EXPECT_TRUE(input->isKeyDown(KeyCode::W));
}