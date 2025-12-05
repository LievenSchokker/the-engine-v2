#include "Events/EventDispatcher.h"

namespace Events
{

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
		vec.erase(
			std::remove_if(vec.begin(), vec.end(),
				[&](const Subscription& s) { return s.id == handle.id; }),
			vec.end()
		);
	}

	handle.invalidate();
}

void EventDispatcher::unsubscribeAll()
{
	listeners.clear();
}

} // namespace Events