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
	Subscription() = default;

	Subscription(EventDispatcher* dispatcher, SubscriptionHandle handle)
		: dispatcher(dispatcher)
		  , handle(handle)
	{
	}

	Subscription(const Subscription&) = delete;
	Subscription& operator=(const Subscription&) = delete;

	Subscription(Subscription&& other) noexcept
		: dispatcher(other.dispatcher)
		  , handle(other.handle)
	{
		other.dispatcher = nullptr;
		other.handle = SubscriptionHandle{};
	}

	Subscription& operator=(Subscription&& other) noexcept
	{
		if (this != &other)
		{
			unsubscribe();
			dispatcher = other.dispatcher;
			handle = other.handle;
			other.dispatcher = nullptr;
			other.handle = SubscriptionHandle{};
		}
		return *this;
	}

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