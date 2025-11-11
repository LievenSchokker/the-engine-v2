#pragma once
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "../enums/KeyCode.h"
#include "../enums/MouseButton.h"
#include <memory>
#include <unordered_set>


class IInputAdapter;

class InputManager
{
  public:
    static InputManager* getInstance();
    static void shutdown();

    /// Configure the adapter used to gather input events.
    void setAdapter(std::unique_ptr<IInputAdapter> newAdapter);

    /// Polling entry point (owned adapter will feed events)
    void update();

    /// Feed raw input during the frame
    void setKeyDown(KeyCode key, bool isDown);
    void setMouseDown(MouseButton button, bool isDown);
    void setMousePosition(int x, int y);
    void addMouseWheelDelta(int dx, int dy); // accumulate per frame
    /// Application quit
    void requestQuit();
    bool shouldQuit() const;

    /// Keyboard queries
    bool isKeyDown(KeyCode key) const;
    bool wasKeyPressed(
        KeyCode key) const; // true only on frames the key transitioned up->down
    bool wasKeyReleased(
        KeyCode key) const; // true only on frames the key transitioned down->up

    /// Mouse button queries
    bool isMouseDown(MouseButton button) const;
    bool wasMousePressed(MouseButton button) const;
    bool wasMouseReleased(MouseButton button) const;

    /// Mouse position and deltas
    int mouseX() const;
    int mouseY() const;
    int mouseDeltaX() const;
    int mouseDeltaY() const;
    bool mouseMovedThisFrame() const;

    /// Mouse wheel deltas accumulated this frame
    int wheelDeltaX() const;
    int wheelDeltaY() const;

  private:
    InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(InputManager&&) = delete;
    ~InputManager();
    std::unique_ptr<IInputAdapter> adapter;
    static InputManager* instance;

    /// Frame lifecycle internals
    void resetPerFrameState();
    void computeTransitions();

    // Keyboard state
    std::unordered_set<KeyCode> keysCurrent;
    std::unordered_set<KeyCode> keysPrevious;
    std::unordered_set<KeyCode> keysPressed;  /// computed in computeTransitions
    std::unordered_set<KeyCode> keysReleased; /// computed in computeTransitions

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
    bool quitRequested = false;
};

#endif // INPUT_MANAGER_H
