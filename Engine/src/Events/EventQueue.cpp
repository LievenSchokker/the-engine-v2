#include "Events/EventQueue.h"

namespace Events
{

void EventQueue::processAll(EventDispatcher& dispatcher)
{
	std::queue<EventAction> toProcess;

	{
		std::lock_guard<std::mutex> lock(mutex);
		std::swap(toProcess, pendingEvents);
	}

	while (!toProcess.empty())
	{
		toProcess.front()(dispatcher);
		toProcess.pop();
	}
}

std::size_t EventQueue::processN(EventDispatcher& dispatcher, std::size_t maxEvents)
{
	std::vector<EventAction> toProcess;
	toProcess.reserve(maxEvents);

	{
		std::lock_guard<std::mutex> lock(mutex);

		std::size_t count = std::min(maxEvents, pendingEvents.size());
		for (std::size_t i = 0; i < count; ++i)
		{
			toProcess.push_back(std::move(pendingEvents.front()));
			pendingEvents.pop();
		}
	}

	for (auto& action : toProcess)
	{
		action(dispatcher);
	}

	return toProcess.size();
}

void EventQueue::clear()
{
	std::lock_guard<std::mutex> lock(mutex);
	std::queue<EventAction> empty;
	std::swap(pendingEvents, empty);
}

bool EventQueue::empty() const
{
	std::lock_guard<std::mutex> lock(mutex);
	return pendingEvents.empty();
}

std::size_t EventQueue::size() const
{
	std::lock_guard<std::mutex> lock(mutex);
	return pendingEvents.size();
}

} // namespace Events