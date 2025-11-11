#include "../../include/input/InputManager.h"
#include "../../include/input/SDLInputAdapter.h"

InputManager *InputManager::instance = nullptr;

InputManager *InputManager::GetInstance()
{
    if (!instance)
    {
        instance = new InputManager();
        instance->_adapter = new SDLInputAdapter();
    }
    return instance;
}

InputManager::~InputManager()
{
    delete _adapter;
    _adapter = nullptr;
}

void InputManager::update()
{
    resetPerFrameState();
    if (_adapter)
    {
        _adapter->poll(*this);
    }
    computeTransitions();
}

void InputManager::requestQuit()
{
    _quitRequested = true;
}

bool InputManager::shouldQuit() const
{
    return _quitRequested;
}

void InputManager::resetPerFrameState()
{
    _keysPressed.clear();
    _keysReleased.clear();
    _mousePressed.clear();
    _mouseReleased.clear();

    _prevMouseX = _mouseX;
    _prevMouseY = _mouseY;
    _mouseMovedThisFrame = false;

    _wheelX = 0;
    _wheelY = 0;
}

void InputManager::computeTransitions()
{
    _keysPressed.clear();
    _keysReleased.clear();
    for (const auto &k : _keysCurrent)
    {
        if (_keysPrevious.find(k) == _keysPrevious.end())
            _keysPressed.insert(k);
    }
    for (const auto &k : _keysPrevious)
    {
        if (_keysCurrent.find(k) == _keysCurrent.end())
            _keysReleased.insert(k);
    }
    _keysPrevious = _keysCurrent;

    _mousePressed.clear();
    _mouseReleased.clear();
    for (const auto &b : _mouseCurrent)
    {
        if (_mousePrevious.find(b) == _mousePrevious.end())
            _mousePressed.insert(b);
    }
    for (const auto &b : _mousePrevious)
    {
        if (_mouseCurrent.find(b) == _mouseCurrent.end())
            _mouseReleased.insert(b);
    }
    _mousePrevious = _mouseCurrent;

    _mouseMovedThisFrame = (_mouseX != _prevMouseX) || (_mouseY != _prevMouseY);
}

void InputManager::setKeyDown(KeyCode key, bool isDown)
{
    if (isDown)
        _keysCurrent.insert(key);
    else
        _keysCurrent.erase(key);
}

void InputManager::setMouseDown(MouseButton button, bool isDown)
{
    if (isDown)
        _mouseCurrent.insert(button);
    else
        _mouseCurrent.erase(button);
}

void InputManager::setMousePosition(int x, int y)
{
    _mouseX = x;
    _mouseY = y;
}

void InputManager::addMouseWheelDelta(int dx, int dy)
{
    _wheelX += dx;
    _wheelY += dy;
}

bool InputManager::isKeyDown(KeyCode key) const
{
    return _keysCurrent.find(key) != _keysCurrent.end();
}

bool InputManager::wasKeyPressed(KeyCode key) const
{
    return _keysPressed.find(key) != _keysPressed.end();
}

bool InputManager::wasKeyReleased(KeyCode key) const
{
    return _keysReleased.find(key) != _keysReleased.end();
}

bool InputManager::isMouseDown(MouseButton button) const
{
    return _mouseCurrent.find(button) != _mouseCurrent.end();
}

bool InputManager::wasMousePressed(MouseButton button) const
{
    return _mousePressed.find(button) != _mousePressed.end();
}

bool InputManager::wasMouseReleased(MouseButton button) const
{
    return _mouseReleased.find(button) != _mouseReleased.end();
}