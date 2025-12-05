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

/**
 * @brief CRTP base for type-safe events
 *
 * Usage:
 *   struct MyEvent : public Event<MyEvent> { ... };
 */
template <typename T>
class Event: public EventBase
{
public:
	static uint32_t staticTypeId()
	{
		static uint32_t id = generateTypeId();
		return id;
	}

	uint32_t getTypeId() const override
	{
		return staticTypeId();
	}
};