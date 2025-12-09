#pragma once


#include "SubscriptionScope.h"

template <typename EventType, typename F>
void SubscriptionScope::subscribe(EventDispatcher& dispatcher, F&& callback)
{
	SubscriptionHandle handle = dispatcher.subscribe<EventType>(std::forward<F>(callback));
	subscriptions.push_back({&dispatcher, handle});
}