#pragma once

#include "Events/Event.h"
#include <cstdint>

struct UIButtonClickedEvent : public Event<UIButtonClickedEvent>
{
	uint32_t buttonId;

	explicit UIButtonClickedEvent(uint32_t id) : buttonId(id) {}
};

struct UIButtonHoveredEvent : public Event<UIButtonHoveredEvent>
{
	uint32_t buttonId;
	bool entered;

	UIButtonHoveredEvent(uint32_t id, bool entered)
		: buttonId(id), entered(entered) {}
};