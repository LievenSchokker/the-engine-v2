#pragma once

#include "KeyCode.h"
#include "MouseButton.h"
#include "../Events/EventDispatcher/EventDispatcher.h"
#include "Core/IEngineSystems.h"

#include <memory>
#include <unordered_set>


struct KeyPressedEvent;
struct KeyReleasedEvent;
struct MouseMovedEvent;
struct MouseButtonPressedEvent;
struct MouseButtonReleasedEvent;
struct MouseScrollEvent;
struct WindowCloseEvent;


/**
 * @brief Central coordinator for game input state.
 *
 * `InputManager` owns the active input adapter and collects raw device events.
 * It exposes high-level queries for keys, mouse buttons, position, wheel
 * deltas, and quit requests so gameplay code can react in the current frame.
 */
class InputManager: public IEngineSystems
{
public:
	InputManager();
	/**
	 * @brief Destroy the singleton instance and release resources.
	 *
	 * Call this during shutdown to clean up the owned adapter and internal
	 * state.
	 */
	static void shutdown();
	void initialize(EventDispatcher& dispatcher);
	SystemStatus start(GameWorld& gameWorld) override;
	void update(double deltaTime, const GameWorld& gameWorld) override;

	/**
	 * @brief Latch a quit request coming from the platform layer.
	 *
	 * Used by adapters to propagate window close or OS quit events; once
	 * signaled, the request remains active until shutdown.
	 */
	void signalQuit();

	/**
	 * @brief Check whether a quit request is currently latched.
	 *
	 * @return True if any previous call to `signalQuit()` has occurred.
	 */
	bool quitRequested() const;

	/**
	 * @brief Test if a key is currently held down.
	 *
	 * @param key Logical key identifier.
	 * @return True if the key is down in the active frame.
	 */
	bool isKeyDown(KeyCode key) const;

	/**
	 * @brief Check if a key was pressed (released to pressed transition).
	 *
	 * @param key Logical key identifier.
	 * @return True only on frames where the key transitioned up-to-down.
	 */
	bool wasKeyPressed(KeyCode key) const;

	/**
	 * @brief Check if a key was released (pressed to released transition).
	 *
	 * @param key Logical key identifier.
	 * @return True only on frames where the key transitioned down-to-up.
	 */
	bool wasKeyReleased(KeyCode key) const;

	/**
	 * @brief Test if a mouse button is currently held down.
	 *
	 * @param button Mouse button identifier.
	 * @return True if the button is down in the active frame.
	 */
	bool isMouseDown(MouseButton button) const;

	/**
	 * @brief Check if a mouse button was pressed this frame.
	 *
	 * @param button Mouse button identifier.
	 * @return True only on frames where the button transitioned up-to-down.
	 */
	bool wasMousePressed(MouseButton button) const;

	/**
	 * @brief Check if a mouse button was released this frame.
	 *
	 * @param button Mouse button identifier.
	 * @return True only on frames where the button transitioned down-to-up.
	 */
	bool wasMouseReleased(MouseButton button) const;

	/**
	 * @brief Get the current mouse X coordinate.
	 *
	 * @return Mouse X position in pixels.
	 */
	int mouseX() const;

	/**
	 * @brief Get the current mouse Y coordinate.
	 *
	 * @return Mouse Y position in pixels.
	 */
	int mouseY() const;

	/**
	 * @brief Get the horizontal mouse movement since the previous frame.
	 *
	 * @return Delta X in pixels.
	 */
	int mouseDeltaX() const;

	/**
	 * @brief Get the vertical mouse movement since the previous frame.
	 *
	 * @return Delta Y in pixels.
	 */
	int mouseDeltaY() const;

	/**
	 * @brief Check if the mouse moved at all this frame.
	 *
	 * @return True if any mouse position change was recorded.
	 */
	bool mouseMovedThisFrame() const;

	/**
	 * @brief Retrieve the accumulated horizontal mouse wheel delta.
	 *
	 * @return Wheel delta X for the current frame.
	 */
	int wheelDeltaX() const;

	/**
	 * @brief Retrieve the accumulated vertical mouse wheel delta.
	 *
	 * @return Wheel delta Y for the current frame.
	 */
	int wheelDeltaY() const;

	const std::string getName() const override;
private:
	void disconnect(EventDispatcher& dispatcher);
	void onKeyPressed(const KeyPressedEvent& e);
	void onKeyReleased(const KeyReleasedEvent& e);
	void onMouseMoved(const MouseMovedEvent& e);
	void onMouseButtonPressed(const MouseButtonPressedEvent& e);
	void onMouseButtonReleased(const MouseButtonReleasedEvent& e);
	void onMouseScroll(const MouseScrollEvent& e);
	void onWindowClose(const WindowCloseEvent& e);

	SubscriptionHandle keyPressedHandle{0, 0};
	SubscriptionHandle keyReleasedHandle{0, 0};
	SubscriptionHandle mouseMovedHandle{0, 0};
	SubscriptionHandle mousePressedHandle{0, 0};
	SubscriptionHandle mouseReleasedHandle{0, 0};
	SubscriptionHandle mouseScrollHandle{0, 0};
	SubscriptionHandle windowCloseHandle{0, 0};
	bool initialized = false;

	std::unordered_set<KeyCode> keysCurrent;
	std::unordered_set<KeyCode> keysPrevious;
	std::unordered_set<KeyCode> keysPressed;
	std::unordered_set<KeyCode> keysReleased;

	// Mouse button state
	std::unordered_set<MouseButton> mouseCurrent;
	std::unordered_set<MouseButton> mousePrevious;
	std::unordered_set<MouseButton> mousePressed;
	std::unordered_set<MouseButton> mouseReleased;

	// Mouse position
	int currentMouseX = 0;
	int currentMouseY = 0;
	int previousMouseX = 0;
	int previousMouseY = 0;
	bool mouseMovedInFrame = false;

	// Mouse wheel (per frame)
	int wheelX = 0;
	int wheelY = 0;
	bool quitSignaled = false;

	EventDispatcher* cachedDispatcher = nullptr;
};