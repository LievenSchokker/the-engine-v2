#pragma once
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "../enums/KeyCode.h"
#include "../enums/MouseButton.h"
#include <unordered_set>


class IInputAdapter;

class InputManager
{
  public:
    static InputManager* GetInstance();

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
    ~InputManager();
    IInputAdapter* _adapter = nullptr;
    static InputManager* instance;

    /// Frame lifecycle internals
    void resetPerFrameState();
    void computeTransitions();

    // Keyboard state
    std::unordered_set<KeyCode> _keysCurrent;
    std::unordered_set<KeyCode> _keysPrevious;
    std::unordered_set<KeyCode> _keysPressed; /// computed in computeTransitions
    std::unordered_set<KeyCode>
        _keysReleased; /// computed in computeTransitions

    // Mouse button state
    std::unordered_set<MouseButton> _mouseCurrent;
    std::unordered_set<MouseButton> _mousePrevious;
    std::unordered_set<MouseButton>
        _mousePressed; // computed in computeTransitions
    std::unordered_set<MouseButton>
        _mouseReleased; // computed in computeTransitions

    // Mouse position
    int _mouseX = 0;
    int _mouseY = 0;
    int _prevMouseX = 0;
    int _prevMouseY = 0;
    bool _mouseMovedThisFrame = false;

    // Mouse wheel (per frame)
    int _wheelX = 0;
    int _wheelY = 0;
    bool _quitRequested = false;
};

#endif // INPUT_MANAGER_H
