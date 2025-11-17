#include "../../inc/Input/InputManager.h"
#include "../../inc/Input/KeyCode.h"
#include "../../inc/Input/MouseButton.h"
#include <SDL.h>
#include <gtest/gtest.h>

class InputManagerTest : public ::testing::Test
{
protected:
  InputManager *input;

  void
  SetUp() override
  {
    // Initialize SDL for testing
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
      FAIL() << "SDL_Init failed: " << SDL_GetError();
    }

    // Get singleton instance
    input = InputManager::getInstance();
  }

  void
  TearDown() override
  {
    // Clean up singleton
    InputManager::shutdown();
    SDL_Quit();
  }
};

// Check input transition detection (pressed/released for keys and mouse)
TEST_F(InputManagerTest, InputTransitions)
{
  // Arrange
  input->update();

  // Act & Assert - Key press transition
  input->setKeyDown(KeyCode::W, true);
  input->update();
  EXPECT_TRUE(input->wasKeyPressed(KeyCode::W));
  EXPECT_TRUE(input->isKeyDown(KeyCode::W));

  // Act & Assert - Key release transition
  input->setKeyDown(KeyCode::W, false);
  input->update();
  EXPECT_TRUE(input->wasKeyReleased(KeyCode::W));
  EXPECT_FALSE(input->isKeyDown(KeyCode::W));

  // Act & Assert - Mouse button press transition
  input->setMouseDown(MouseButton::LEFT, true);
  input->update();
  EXPECT_TRUE(input->wasMousePressed(MouseButton::LEFT));
  EXPECT_TRUE(input->isMouseDown(MouseButton::LEFT));

  // Act & Assert - Mouse button release transition
  input->setMouseDown(MouseButton::LEFT, false);
  input->update();
  EXPECT_TRUE(input->wasMouseReleased(MouseButton::LEFT));
  EXPECT_FALSE(input->isMouseDown(MouseButton::LEFT));
}

// Check mouse delta (movement since previous frame)
TEST_F(InputManagerTest, MouseDelta)
{
  // Arrange
  input->setMousePosition(10, 10);
  input->update();

  // Act
  input->setMousePosition(30, 40);

  // Assert
  EXPECT_EQ(input->mouseDeltaX(), 20);
  EXPECT_EQ(input->mouseDeltaY(), 30);
  EXPECT_EQ(input->mouseX(), 30);
  EXPECT_EQ(input->mouseY(), 40);

  // Act & Assert - Delta resets after update
  input->update();
  EXPECT_EQ(input->mouseDeltaX(), 0);
  EXPECT_EQ(input->mouseDeltaY(), 0);
}

// Check mouse wheel delta (including accumulation and reset)
TEST_F(InputManagerTest, MouseWheelDelta)
{
  // Act & Assert - Single wheel event
  input->addMouseWheelDelta(5, 10);
  EXPECT_EQ(input->wheelDeltaX(), 5);
  EXPECT_EQ(input->wheelDeltaY(), 10);

  // Act & Assert - Multiple events accumulate
  input->addMouseWheelDelta(2, 3);
  input->addMouseWheelDelta(3, 4);
  EXPECT_EQ(input->wheelDeltaX(), 10);
  EXPECT_EQ(input->wheelDeltaY(), 17);

  // Act & Assert - Reset after update
  input->update();
  EXPECT_EQ(input->wheelDeltaX(), 0);
  EXPECT_EQ(input->wheelDeltaY(), 0);
}

// Multiple inputs, transitions, frame reset, and quit
TEST_F(InputManagerTest, MultipleInputsAndFrameReset)
{
  // Arrange - Multiple keys and mouse buttons
  input->setKeyDown(KeyCode::W, true);
  input->setKeyDown(KeyCode::A, true);
  input->setMouseDown(MouseButton::LEFT, true);
  input->update();

  // Assert - Verify held states
  EXPECT_TRUE(input->isKeyDown(KeyCode::W));
  EXPECT_TRUE(input->isKeyDown(KeyCode::A));
  EXPECT_TRUE(input->isMouseDown(MouseButton::LEFT));

  // Act - Add new input (should detect transition)
  input->setMouseDown(MouseButton::RIGHT, true);
  input->update();

  // Assert - Transition detected
  EXPECT_TRUE(input->wasMousePressed(MouseButton::RIGHT));
  EXPECT_TRUE(input->isMouseDown(MouseButton::RIGHT));

  // Act & Assert - Transition states reset after update
  input->update();
  EXPECT_FALSE(input->wasMousePressed(MouseButton::RIGHT));
  EXPECT_FALSE(input->wasKeyPressed(KeyCode::W));

  // Act & Assert - Quit request
  EXPECT_FALSE(input->quitRequested());
  input->signalQuit();
  EXPECT_TRUE(input->quitRequested());
}
