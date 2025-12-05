#pragma once

#include "KeyCode.h"
#include "MouseButton.h"
#include "Events/EventDispatcher.h"

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
class InputManager
{
public:
	/**
	 * @brief Retrieve the singleton instance.
	 *
	 * @return Pointer to the global `InputManager`.
	 */
	static InputManager* getInstance();

	/**
	 * @brief Destroy the singleton instance and release resources.
	 *
	 * Call this during shutdown to clean up the owned adapter and internal
	 * state.
	 */
	static void shutdown();
	void initialize(EventDispatcher& dispatcher);

	/**
	 * @brief Poll for new input events and update cached state.
	 *
	 * Call once per frame; the adapter will feed fresh events into the manager.
	 */
	void update();

	/**
	 * @brief Record a key state change for the current frame.
	 *
	 * Typically used by an input adapter to push low-level events.
	 *
	 * @param key Logical key identifier.
	 * @param isDown True if the key is currently pressed.
	 */
	void setKeyDown(KeyCode key, bool isDown);

	/**
	 * @brief Record a mouse button state change for the current frame.
	 *
	 * @param button Mouse button identifier.
	 * @param isDown True if the button is currently pressed.
	 */
	void setMouseDown(MouseButton button, bool isDown);

	/**
	 * @brief Update the tracked mouse cursor position.
	 *
	 * @param x Horizontal screen coordinate.
	 * @param y Vertical screen coordinate.
	 */
	void setMousePosition(int x, int y);

	/**
	 * @brief Accumulate mouse wheel delta for the current frame.
	 *
	 * Multiple calls within a frame are summed.
	 *
	 * @param dx Horizontal wheel delta.
	 * @param dy Vertical wheel delta.
	 */
	void addMouseWheelDelta(int dx, int dy);

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

	void beginFrame();
	void endFrame();
private:
	InputManager() = default;
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;
	~InputManager() = default;
	static InputManager* instance;

	void disconnect(EventDispatcher& dispatcher);
	void onKeyPressed(const KeyPressedEvent& e);
	void onKeyReleased(const KeyReleasedEvent& e);
	void onMouseMoved(const MouseMovedEvent& e);
	void onMouseButtonPressed(const MouseButtonPressedEvent& e);
	void onMouseButtonReleased(const MouseButtonReleasedEvent& e);
	void onMouseScroll(const MouseScrollEvent& e);
	void onWindowClose(const WindowCloseEvent& e);

	SubscriptionHandle keyPressedHandle;
	SubscriptionHandle keyReleasedHandle;
	SubscriptionHandle mouseMovedHandle;
	SubscriptionHandle mousePressedHandle;
	SubscriptionHandle mouseReleasedHandle;
	SubscriptionHandle mouseScrollHandle;
	SubscriptionHandle windowCloseHandle;
	bool initialized = false;

	// Keyboard state
	std::unordered_set<KeyCode> keysCurrent;
	std::unordered_set<KeyCode> keysPrevious;
	std::unordered_set<KeyCode> keysPressed; // computed in computeTransitions
	std::unordered_set<KeyCode> keysReleased; // computed in computeTransitions

	// Mouse button state
	std::unordered_set<MouseButton> mouseCurrent;
	std::unordered_set<MouseButton> mousePrevious;
	std::unordered_set<MouseButton>
	mousePressed; // computed in computeTransitions
	std::unordered_set<MouseButton>
	mouseReleased; // computed in computeTransitions

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
};