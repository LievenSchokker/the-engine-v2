#include "../../inc/Events/EventDispatcher/EventDispatcher.h"

void EventDispatcher::unsubscribe(SubscriptionHandle& handle)
{
	if (!handle.isValid())
	{
		return;
	}

	auto subscriptionIterator = listeners.find(handle.typeId);
	if (subscriptionIterator != listeners.end())
	{
		auto& subscriptionVector = subscriptionIterator->second;
		std::erase_if(subscriptionVector,
		              [&](const Subscription& s) { return s.id == handle.id; });
	}

	handle.invalidate();
}

void EventDispatcher::unsubscribeAll()
{
	listeners.clear();
}

