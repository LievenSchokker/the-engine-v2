#pragma once


#include "Events/Event.h"
#include "Input/MouseButton.h"

struct WindowResizeEvent: public Event<WindowResizeEvent>
{
	int width;
	int height;

	WindowResizeEvent(int w, int h)
		: width(w), height(h)
	{
	}
};

struct WindowCloseEvent: public Event<WindowCloseEvent>
{
	WindowCloseEvent() = default;
};

struct WindowFocusEvent: public Event<WindowFocusEvent>
{
	bool focused;

	explicit WindowFocusEvent(bool focused)
		: focused(focused)
	{
	}
};

struct KeyPressedEvent: public Event<KeyPressedEvent>
{
	int keyCode;
	int scanCode;
	bool isRepeat;

	KeyPressedEvent(int keyCode, int scanCode, bool repeat = false)
		: keyCode(keyCode), scanCode(scanCode), isRepeat(repeat)
	{
	}
};

struct KeyReleasedEvent: public Event<KeyReleasedEvent>
{
	int keyCode;
	int scanCode;

	KeyReleasedEvent(int keyCode, int scanCode)
		: keyCode(keyCode), scanCode(scanCode)
	{
	}
};


struct MouseMovedEvent: public Event<MouseMovedEvent>
{
	int x;
	int y;
	int deltaX;
	int deltaY;

	MouseMovedEvent(int x, int y, int dx, int dy)
		: x(x), y(y), deltaX(dx), deltaY(dy)
	{
	}
};

struct MouseButtonPressedEvent: public Event<MouseButtonPressedEvent>
{
	MouseButton button;
	int x;
	int y;
	int clicks;

	MouseButtonPressedEvent(MouseButton button, int x, int y, int clicks = 1)
		: button(button), x(x), y(y), clicks(clicks)
	{
	}
};

struct MouseButtonReleasedEvent: public Event<MouseButtonReleasedEvent>
{
	MouseButton button;
	int x;
	int y;

	MouseButtonReleasedEvent(MouseButton button, int x, int y)
		: button(button), x(x), y(y)
	{
	}
};

struct MouseScrollEvent: public Event<MouseScrollEvent>
{
	float deltaX;
	float deltaY;

	MouseScrollEvent(float dx, float dy)
		: deltaX(dx), deltaY(dy)
	{
	}
};