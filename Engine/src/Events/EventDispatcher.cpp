#include "Events/EventDispatcher.h"

void EventDispatcher::unsubscribe(SubscriptionHandle& handle)
{
	if (!handle.isValid())
	{
		return;
	}

	auto it = listeners.find(handle.typeId);
	if (it != listeners.end())
	{
		auto& vec = it->second;
		std::erase_if(vec,
		              [&](const Subscription& s) { return s.id == handle.id; });
	}

	handle.invalidate();
}

void EventDispatcher::unsubscribeAll()
{
	listeners.clear();
}

