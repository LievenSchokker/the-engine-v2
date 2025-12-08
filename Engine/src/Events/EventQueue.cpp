#include "Events/EventQueue.h"

void EventQueue::processAll(EventDispatcher& dispatcher)
{
	std::queue<EventAction> toProcess;

	std::swap(toProcess, pendingEvents);

	while (!toProcess.empty())
	{
		toProcess.front()(dispatcher);
		toProcess.pop();
	}
}

void EventQueue::clear()
{
	std::queue<EventAction> empty;
	std::swap(pendingEvents, empty);
}

bool EventQueue::empty() const
{
	return pendingEvents.empty();
}

std::size_t EventQueue::size() const
{
	return pendingEvents.size();
}