#pragma once

#include <cstdint>

/**
 * @brief Base class for all events with automatic type ID generation
 */
class EventBase
{
public:
	virtual ~EventBase() = default;

	/// Returns unique type ID for this event type
	virtual uint32_t getTypeId() const = 0;

	/// Whether the event has been handled (stops propagation if true)
	bool handled = false;

protected:
	static uint32_t generateTypeId()
	{
		static uint32_t nextId = 0;
		return nextId++;
	}
};

#include "Event.inl"