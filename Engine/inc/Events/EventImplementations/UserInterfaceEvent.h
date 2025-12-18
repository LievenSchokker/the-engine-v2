#pragma once

#include "Events/Event.h"
#include <cstdint>

struct UIButtonClickedEvent final : public Event<UIButtonClickedEvent>
{
	uint32_t buttonId;

	explicit UIButtonClickedEvent(const uint32_t id) : buttonId(id) {}
};

struct UIButtonHoveredEvent final : public Event<UIButtonHoveredEvent>
{
	uint32_t buttonId;
	bool entered;

	UIButtonHoveredEvent(const uint32_t id, const bool entered)
		: buttonId(id), entered(entered) {}
};