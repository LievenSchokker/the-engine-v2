#pragma once

class IEventProccesor
{
public:
	 virtual ~IEventProccesor() = default;

	/**
	 * @brief Poll and process all SDL events, dispatching immediately
	 * @param dispatcher Event dispatcher
	 * @return false if application should quit
	 */
	virtual bool pollEvents(EventDispatcher& dispatcher) = 0;
};