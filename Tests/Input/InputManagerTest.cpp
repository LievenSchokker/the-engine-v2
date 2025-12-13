#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"
#include "Events/EventDispatcher/EventDispatcher.h"
#include "Events/EventImplementations/ApplicationEvents.h"
#include <gtest/gtest.h>

class InputManagerTest : public ::testing::Test
{
protected:
    InputManager* input;
    EventDispatcher dispatcher;

    void SetUp() override
    {
        input = InputManager::getInstance();
        input->initialize(dispatcher);
    }

    void TearDown() override
    {
        InputManager::shutdown();
    }

    void pressKey(KeyCode key)
    {
        KeyPressedEvent event(0, static_cast<int>(key), false);
        dispatcher.dispatch(event);
    }

    void releaseKey(KeyCode key)
    {
        KeyReleasedEvent event(0, static_cast<int>(key));
        dispatcher.dispatch(event);
    }

    void pressMouseButton(MouseButton button)
    {
        MouseButtonPressedEvent event(button, 0, 0, 1);
        dispatcher.dispatch(event);
    }

    void releaseMouseButton(MouseButton button)
    {
        MouseButtonReleasedEvent event(button, 0, 0);
        dispatcher.dispatch(event);
    }

    void moveMouse(int x, int y)
    {
        MouseMovedEvent event(x, y, 0, 0);
        dispatcher.dispatch(event);
    }

    void scrollMouse(float dx, float dy)
    {
        MouseScrollEvent event(dx, dy);
        dispatcher.dispatch(event);
    }

    void closeWindow()
    {
        WindowCloseEvent event;
        dispatcher.dispatch(event);
    }
};

TEST_F(InputManagerTest, InputTransitions)
{
    // Frame 1: Press key
    input->beginFrame();
    pressKey(KeyCode::W);
    input->endFrame();

    EXPECT_TRUE(input->wasKeyPressed(KeyCode::W));
    EXPECT_TRUE(input->isKeyDown(KeyCode::W));

    // Frame 2: Key held (no new press)
    input->beginFrame();
    input->endFrame();

    EXPECT_FALSE(input->wasKeyPressed(KeyCode::W));  // Not newly pressed
    EXPECT_TRUE(input->isKeyDown(KeyCode::W));       // Still held

    // Frame 3: Release key
    input->beginFrame();
    releaseKey(KeyCode::W);
    input->endFrame();

    EXPECT_TRUE(input->wasKeyReleased(KeyCode::W));
    EXPECT_FALSE(input->isKeyDown(KeyCode::W));

    // Frame 4: Mouse button press
    input->beginFrame();
    pressMouseButton(MouseButton::LEFT);
    input->endFrame();

    EXPECT_TRUE(input->wasMousePressed(MouseButton::LEFT));
    EXPECT_TRUE(input->isMouseDown(MouseButton::LEFT));

    // Frame 5: Mouse button release
    input->beginFrame();
    releaseMouseButton(MouseButton::LEFT);
    input->endFrame();

    EXPECT_TRUE(input->wasMouseReleased(MouseButton::LEFT));
    EXPECT_FALSE(input->isMouseDown(MouseButton::LEFT));
}

TEST_F(InputManagerTest, MouseDelta)
{
    // Frame 1: Set initial position
    input->beginFrame();
    moveMouse(10, 10);
    input->endFrame();

    // Frame 2: Move mouse
    input->beginFrame();
    moveMouse(30, 40);
    input->endFrame();

    EXPECT_EQ(input->mouseDeltaX(), 20);
    EXPECT_EQ(input->mouseDeltaY(), 30);
    EXPECT_EQ(input->mouseX(), 30);
    EXPECT_EQ(input->mouseY(), 40);

    // Frame 3: No movement - delta resets
    input->beginFrame();
    input->endFrame();

    EXPECT_EQ(input->mouseDeltaX(), 0);
    EXPECT_EQ(input->mouseDeltaY(), 0);
}

TEST_F(InputManagerTest, MouseWheelDelta)
{
    input->beginFrame();

    scrollMouse(5, 10);
    EXPECT_EQ(input->wheelDeltaX(), 5);
    EXPECT_EQ(input->wheelDeltaY(), 10);

    // Multiple events accumulate within frame
    scrollMouse(2, 3);
    scrollMouse(3, 4);
    EXPECT_EQ(input->wheelDeltaX(), 10);
    EXPECT_EQ(input->wheelDeltaY(), 17);

    input->endFrame();

    // Reset after new frame
    input->beginFrame();
    EXPECT_EQ(input->wheelDeltaX(), 0);
    EXPECT_EQ(input->wheelDeltaY(), 0);
}

TEST_F(InputManagerTest, MultipleInputsAndFrameReset)
{
    // Frame 1: Multiple inputs
    input->beginFrame();
    pressKey(KeyCode::W);
    pressKey(KeyCode::A);
    pressMouseButton(MouseButton::LEFT);
    input->endFrame();

    EXPECT_TRUE(input->isKeyDown(KeyCode::W));
    EXPECT_TRUE(input->isKeyDown(KeyCode::A));
    EXPECT_TRUE(input->isMouseDown(MouseButton::LEFT));

    // Frame 2: Add new input
    input->beginFrame();
    pressMouseButton(MouseButton::RIGHT);
    input->endFrame();

    EXPECT_TRUE(input->wasMousePressed(MouseButton::RIGHT));
    EXPECT_TRUE(input->isMouseDown(MouseButton::RIGHT));

    // Frame 3: Transitions reset
    input->beginFrame();
    input->endFrame();

    EXPECT_FALSE(input->wasMousePressed(MouseButton::RIGHT));
    EXPECT_FALSE(input->wasKeyPressed(KeyCode::W));

    // Quit request
    EXPECT_FALSE(input->quitRequested());
    closeWindow();
    EXPECT_TRUE(input->quitRequested());
}