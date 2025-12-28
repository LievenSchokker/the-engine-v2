#include "Input/InputManager.h"
#include "Core/GameWorld.h"
#include "Events/EventImplementations/ApplicationEvents.h"

#include <iostream>
#include <ostream>

InputManager::InputManager() = default;

void InputManager::shutdown()
{}

void InputManager::initialize(EventDispatcher& dispatcher)
{
    if (initialized)
    {
        return;
    }

    cachedDispatcher = &dispatcher;

    keyPressedHandle = dispatcher.subscribe<KeyPressedEvent>(
        [this](const KeyPressedEvent& e) { onKeyPressed(e); });

    keyReleasedHandle = dispatcher.subscribe<KeyReleasedEvent>(
        [this](const KeyReleasedEvent& e) { onKeyReleased(e); });

    mouseMovedHandle = dispatcher.subscribe<MouseMovedEvent>(
        [this](const MouseMovedEvent& e) { onMouseMoved(e); });

    mousePressedHandle = dispatcher.subscribe<MouseButtonPressedEvent>(
        [this](const MouseButtonPressedEvent& e) { onMouseButtonPressed(e); });

    mouseReleasedHandle = dispatcher.subscribe<MouseButtonReleasedEvent>(
        [this](const MouseButtonReleasedEvent& e) { onMouseButtonReleased(e); });

    mouseScrollHandle = dispatcher.subscribe<MouseScrollEvent>(
        [this](const MouseScrollEvent& e) { onMouseScroll(e); });

    windowCloseHandle = dispatcher.subscribe<WindowCloseEvent>(
        [this](const WindowCloseEvent& e) { onWindowClose(e); });

    initialized = true;
}

void InputManager::disconnect(EventDispatcher& dispatcher)
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

SystemStatus InputManager::start(GameWorld& gameWorld)
{
	gameWorld.input = this;

	EventDispatcher* dispatcher = gameWorld.getDispatcher();

	if (dispatcher != nullptr)
	{
		initialize(*dispatcher);
		return SystemStatus::RUNNING;
	}

	return SystemStatus::ERROR;
}

void InputManager::update(double deltaTime, const GameWorld& gameWorld)
{
    keysPressed.clear();
    keysReleased.clear();

    for (const auto& key : keysCurrent)
    {
        if (keysPrevious.find(key) == keysPrevious.end())
            keysPressed.insert(key);
    }
    for (const auto& key : keysPrevious)
    {
        if (keysCurrent.find(key) == keysCurrent.end())
            keysReleased.insert(key);
    }
    keysPrevious = keysCurrent;

    // Same for mouse
    mousePressed.clear();
    mouseReleased.clear();

    for (const auto& button : mouseCurrent)
    {
        if (mousePrevious.find(button) == mousePrevious.end())
            mousePressed.insert(button);
    }
    for (const auto& button : mousePrevious)
    {
        if (mouseCurrent.find(button) == mouseCurrent.end())
            mouseReleased.insert(button);
    }
    mousePrevious = mouseCurrent;

    mouseMovedInFrame = (currentMouseX != previousMouseX) ||
                        (currentMouseY != previousMouseY);
    previousMouseX = currentMouseX;
    previousMouseY = currentMouseY;

    wheelX = 0;
    wheelY = 0;
}


void InputManager::onKeyPressed(const KeyPressedEvent& e)
{
    if (!e.isRepeat)
    {
        keysCurrent.insert(static_cast<KeyCode>(e.scanCode));
    }
}

void InputManager::onKeyReleased(const KeyReleasedEvent& e)
{
    keysCurrent.erase(static_cast<KeyCode>(e.scanCode));
}

void InputManager::onMouseMoved(const MouseMovedEvent& e)
{
    currentMouseX = e.x;
    currentMouseY = e.y;
    mouseMovedInFrame = true;
}

void InputManager::onMouseButtonPressed(const MouseButtonPressedEvent& e)
{
    mouseCurrent.insert(static_cast<MouseButton>(e.button));
}

void InputManager::onMouseButtonReleased(const MouseButtonReleasedEvent& e)
{
    mouseCurrent.erase(static_cast<MouseButton>(e.button));
}

void InputManager::onMouseScroll(const MouseScrollEvent& event)
{
    wheelX += static_cast<int>(event.deltaX);
    wheelY += static_cast<int>(event.deltaY);
}

void InputManager::onWindowClose(const WindowCloseEvent& e)
{
    quitSignaled = true;
}

bool InputManager::quitRequested() const { return quitSignaled; }
bool InputManager::isKeyDown(KeyCode key) const
{
	return keysCurrent.contains(key);
}
bool InputManager::wasKeyPressed(KeyCode key) const { return keysPressed.contains(key); }
bool InputManager::wasKeyReleased(KeyCode key) const { return keysReleased.contains(key); }
bool InputManager::isMouseDown(MouseButton button) const { return mouseCurrent.contains(button); }
bool InputManager::wasMousePressed(MouseButton button) const { return mousePressed.contains(button); }
bool InputManager::wasMouseReleased(MouseButton button) const { return mouseReleased.contains(button); }
int InputManager::mouseX() const { return currentMouseX; }
int InputManager::mouseY() const { return currentMouseY; }
int InputManager::mouseDeltaX() const { return currentMouseX - previousMouseX; }
int InputManager::mouseDeltaY() const { return currentMouseY - previousMouseY; }
bool InputManager::mouseMovedThisFrame() const { return mouseMovedInFrame; }
int InputManager::wheelDeltaX() const { return wheelX; }
int InputManager::wheelDeltaY() const { return wheelY; }
void InputManager::signalQuit() { quitSignaled = true; }

const std::string InputManager::getName() const
{
	return "InputManager";
}
