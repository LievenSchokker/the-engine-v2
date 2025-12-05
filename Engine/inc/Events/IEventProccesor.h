
#include "EventQueue.h"

class IEventProccesor
{
public:
	 virtual ~IEventProccesor() = default;

	/**
	 * @brief Poll and process all SDL events, queuing them
	 * @param queue Event queue to push events into
	 * @return false if application should quit
	 */
	virtual bool pollEvents(EventQueue& queue) = 0;

	/**
	 * @brief Poll and process all SDL events, dispatching immediately
	 * @param dispatcher Event dispatcher
	 * @return false if application should quit
	 */
	virtual bool pollEvents(EventDispatcher& dispatcher) = 0;
};