#pragma once


#include "Events/EventDispatcher.h"


/**
 * @brief RAII wrapper for event subscriptions
 *
 * Automatically unsubscribes when destroyed. Move-only.
 */
class Subscription
{
public:
	Subscription();

	Subscription(EventDispatcher* dispatcher, SubscriptionHandle handle);

	Subscription(Subscription&& other);

	Subscription& operator=(Subscription&& other);

	~Subscription()
	{
		unsubscribe();
	}

	void unsubscribe()
	{
		if (dispatcher && handle.isValid())
		{
			dispatcher->unsubscribe(handle);
			handle = SubscriptionHandle{};
		}
	}

	bool isValid() const
	{
		return handle.isValid();
	}

private:
	EventDispatcher* dispatcher = nullptr;
	SubscriptionHandle handle;
};