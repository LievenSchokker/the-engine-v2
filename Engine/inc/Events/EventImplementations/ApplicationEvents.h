#pragma once


#include "Events/Event.h"
#include "Input/MouseButton.h"

struct WindowResizeEvent final : public Event<WindowResizeEvent>
{
	int width;
	int height;

	WindowResizeEvent(const int w,const int h)
		: width(w), height(h)
	{
	}
};

struct WindowCloseEvent final : public Event<WindowCloseEvent>
{
	WindowCloseEvent() = default;
};

struct WindowFocusEvent final : public Event<WindowFocusEvent>
{
	bool focused;

	explicit WindowFocusEvent(const bool focused)
		: focused(focused)
	{
	}
};

struct KeyPressedEvent: public Event<KeyPressedEvent>
{
	int keyCode;
	int scanCode;
	bool isRepeat;

	KeyPressedEvent(const int keyCode, const int scanCode, const bool repeat = false)
		: keyCode(keyCode), scanCode(scanCode), isRepeat(repeat)
	{
	}
};

struct KeyReleasedEvent final : public Event<KeyReleasedEvent>
{
	int keyCode;
	int scanCode;

	KeyReleasedEvent(const int keyCode, const int scanCode)
		: keyCode(keyCode), scanCode(scanCode)
	{
	}
};


struct MouseMovedEvent final : public Event<MouseMovedEvent>
{
	int x;
	int y;
	int deltaX;
	int deltaY;

	MouseMovedEvent(const int x, const int y, const int dx, const int dy)
		: x(x), y(y), deltaX(dx), deltaY(dy)
	{
	}
};

struct MouseButtonPressedEvent final : public Event<MouseButtonPressedEvent>
{
	MouseButton button;
	int x;
	int y;
	int clicks;

	MouseButtonPressedEvent(const MouseButton button,const int x,const int y,const int clicks = 1)
		: button(button), x(x), y(y), clicks(clicks)
	{
	}
};

struct MouseButtonReleasedEvent final : public Event<MouseButtonReleasedEvent>
{
	MouseButton button;
	int x;
	int y;

	MouseButtonReleasedEvent(const MouseButton button, const int x, const int y)
		: button(button), x(x), y(y)
	{
	}
};

struct MouseScrollEvent final : public Event<MouseScrollEvent>
{
	float deltaX;
	float deltaY;

	MouseScrollEvent(const float dx, const float dy)
		: deltaX(dx), deltaY(dy)
	{
	}
};