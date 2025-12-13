#pragma once


#include "SubscriptionScope.h"

template <typename EventType, typename F>
void SubscriptionScope::subscribe(F&& callback)
{
	if (!dispatcher)
	{
		return;
	}
	handles.push_back(dispatcher->subscribe<EventType>(std::forward<F>(callback)));
}