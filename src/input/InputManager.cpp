#include "../../include/input/InputManager.h"
#include "../../include/input/SDLInputAdapter.h"

InputManager* InputManager::instance = nullptr;

InputManager* InputManager::getInstance()
{
    if (!instance)
    {
        instance = new InputManager();
        instance->adapter = new SDLInputAdapter();
    }
    return instance;
}

void InputManager::shutdown()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}

InputManager::~InputManager()
{
    delete adapter;
    adapter = nullptr;
}

void InputManager::update()
{
    resetPerFrameState();
    if (adapter)
    {
        adapter->poll(*this);
    }
    computeTransitions();
}

void InputManager::requestQuit()
{
    quitRequested = true;
}

bool InputManager::shouldQuit() const
{
    return quitRequested;
}

void InputManager::resetPerFrameState()
{
    keysPressed.clear();
    keysReleased.clear();
    mousePressed.clear();
    mouseReleased.clear();

    previousMouseX = currentMouseX;
    previousMouseY = currentMouseY;
    mouseMovedInFrame = false;

    wheelX = 0;
    wheelY = 0;
}

void InputManager::computeTransitions()
{
    keysPressed.clear();
    keysReleased.clear();
    for (const auto& k : keysCurrent)
    {
        if (keysPrevious.find(k) == keysPrevious.end())
            keysPressed.insert(k);
    }
    for (const auto& k : keysPrevious)
    {
        if (keysCurrent.find(k) == keysCurrent.end())
            keysReleased.insert(k);
    }
    keysPrevious = keysCurrent;

    mousePressed.clear();
    mouseReleased.clear();
    for (const auto& b : mouseCurrent)
    {
        if (mousePrevious.find(b) == mousePrevious.end())
            mousePressed.insert(b);
    }
    for (const auto& b : mousePrevious)
    {
        if (mouseCurrent.find(b) == mouseCurrent.end())
            mouseReleased.insert(b);
    }
    mousePrevious = mouseCurrent;

    mouseMovedInFrame =
        (currentMouseX != previousMouseX) || (currentMouseY != previousMouseY);
}

void InputManager::setKeyDown(KeyCode key, bool isDown)
{
    if (isDown)
        keysCurrent.insert(key);
    else
        keysCurrent.erase(key);
}

void InputManager::setMouseDown(MouseButton button, bool isDown)
{
    if (isDown)
        mouseCurrent.insert(button);
    else
        mouseCurrent.erase(button);
}

void InputManager::setMousePosition(int x, int y)
{
    currentMouseX = x;
    currentMouseY = y;
}

void InputManager::addMouseWheelDelta(int dx, int dy)
{
    wheelX += dx;
    wheelY += dy;
}

bool InputManager::isKeyDown(KeyCode key) const
{
    return keysCurrent.find(key) != keysCurrent.end();
}

bool InputManager::wasKeyPressed(KeyCode key) const
{
    return keysPressed.find(key) != keysPressed.end();
}

bool InputManager::wasKeyReleased(KeyCode key) const
{
    return keysReleased.find(key) != keysReleased.end();
}

bool InputManager::isMouseDown(MouseButton button) const
{
    return mouseCurrent.find(button) != mouseCurrent.end();
}

bool InputManager::wasMousePressed(MouseButton button) const
{
    return mousePressed.find(button) != mousePressed.end();
}

bool InputManager::wasMouseReleased(MouseButton button) const
{
    return mouseReleased.find(button) != mouseReleased.end();
}

int InputManager::mouseX() const
{
    return currentMouseX;
}

int InputManager::mouseY() const
{
    return currentMouseY;
}

int InputManager::mouseDeltaX() const
{
    return currentMouseX - previousMouseX;
}

int InputManager::mouseDeltaY() const
{
    return currentMouseY - previousMouseY;
}

bool InputManager::mouseMovedThisFrame() const
{
    return mouseMovedInFrame;
}

int InputManager::wheelDeltaX() const
{
    return wheelX;
}

int InputManager::wheelDeltaY() const
{
    return wheelY;
}
