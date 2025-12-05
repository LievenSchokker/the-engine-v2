#include "Input/InputManager.h"

#include "Events/ApplicationEvents.h"
#include <SDL_keycode.h>

InputManager* InputManager::instance = nullptr;

InputManager* InputManager::getInstance()
{
    if (!instance)
    {
        instance = new InputManager();
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

void InputManager::initialize(Events::EventDispatcher& dispatcher)
{
    if (initialized)
    {
        return;
    }

    keyPressedHandle = dispatcher.subscribe<Events::KeyPressedEvent>(
        [this](const Events::KeyPressedEvent& e) {
            onKeyPressed(e);
        });

    keyReleasedHandle = dispatcher.subscribe<Events::KeyReleasedEvent>(
        [this](const Events::KeyReleasedEvent& e) {
            onKeyReleased(e);
        });

    mouseMovedHandle = dispatcher.subscribe<Events::MouseMovedEvent>(
        [this](const Events::MouseMovedEvent& e) {
            onMouseMoved(e);
        });

    mousePressedHandle = dispatcher.subscribe<Events::MouseButtonPressedEvent>(
        [this](const Events::MouseButtonPressedEvent& e) {
            onMouseButtonPressed(e);
        });

    mouseReleasedHandle = dispatcher.subscribe<Events::MouseButtonReleasedEvent>(
        [this](const Events::MouseButtonReleasedEvent& e) {
            onMouseButtonReleased(e);
        });

    mouseScrollHandle = dispatcher.subscribe<Events::MouseScrollEvent>(
        [this](const Events::MouseScrollEvent& e) {
            onMouseScroll(e);
        });

    windowCloseHandle = dispatcher.subscribe<Events::WindowCloseEvent>(
        [this](const Events::WindowCloseEvent& e) {
            onWindowClose(e);
        });

    initialized = true;
}

void InputManager::disconnect(Events::EventDispatcher& dispatcher)
{
    if (!initialized)
    {
        return;
    }

    dispatcher.unsubscribe(keyPressedHandle);
    dispatcher.unsubscribe(keyReleasedHandle);
    dispatcher.unsubscribe(mouseMovedHandle);
    dispatcher.unsubscribe(mousePressedHandle);
    dispatcher.unsubscribe(mouseReleasedHandle);
    dispatcher.unsubscribe(mouseScrollHandle);
    dispatcher.unsubscribe(windowCloseHandle);

    initialized = false;
}

void InputManager::beginFrame()
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

void InputManager::endFrame()
{
    for (const auto& key : keysCurrent)
    {
        if (keysPrevious.find(key) == keysPrevious.end())
        {
            keysPressed.insert(key);
        }
    }
    for (const auto& key : keysPrevious)
    {
        if (keysCurrent.find(key) == keysCurrent.end())
        {
            keysReleased.insert(key);
        }
    }
    keysPrevious = keysCurrent;

    for (const auto& button : mouseCurrent)
    {
        if (mousePrevious.find(button) == mousePrevious.end())
        {
            mousePressed.insert(button);
        }
    }
    for (const auto& button : mousePrevious)
    {
        if (mouseCurrent.find(button) == mouseCurrent.end())
        {
            mouseReleased.insert(button);
        }
    }
    mousePrevious = mouseCurrent;

    mouseMovedInFrame = (currentMouseX != previousMouseX) ||
                        (currentMouseY != previousMouseY);
}

void InputManager::onKeyPressed(const Events::KeyPressedEvent& e)
{
    if (!e.isRepeat)
    {
        keysCurrent.insert(static_cast<KeyCode>(e.scanCode));
    }
}

void InputManager::onKeyReleased(const Events::KeyReleasedEvent& e)
{
    keysCurrent.erase(static_cast<KeyCode>(e.scanCode));
}

void InputManager::onMouseMoved(const Events::MouseMovedEvent& e)
{
    currentMouseX = e.x;
    currentMouseY = e.y;
    mouseMovedInFrame = true;
}

void InputManager::onMouseButtonPressed(const Events::MouseButtonPressedEvent& e)
{
    mouseCurrent.insert(static_cast<MouseButton>(e.button));
}

void InputManager::onMouseButtonReleased(const Events::MouseButtonReleasedEvent& e)
{
    mouseCurrent.erase(static_cast<MouseButton>(e.button));
}

void InputManager::onMouseScroll(const Events::MouseScrollEvent& e)
{
    wheelX += static_cast<int>(e.deltaX);
    wheelY += static_cast<int>(e.deltaY);
}

void InputManager::onWindowClose(const Events::WindowCloseEvent& e)
{
    quitSignaled = true;
}

bool InputManager::quitRequested() const
{
    return quitSignaled;
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