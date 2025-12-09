#pragma once


#include "Behaviour.h"

#include <iostream>

template <typename EventType, typename T>
void Behaviour::subscribe(void (T::*method)(const EventType&) const)
{
	static_assert(std::is_base_of_v<Behaviour, T>, "T must derive from Behaviour");

	if (dispatcher == nullptr) return;

	const T* self = static_cast<const T*>(this);

	subscriptions->subscribe<EventType>(*dispatcher,
		[self, method](const EventType& e)
		{
			(self->*method)(e);
		});
}

template <typename EventType, typename T>
void Behaviour::subscribe(void (T::*method)(const EventType&))
{
	static_assert(std::is_base_of_v<Behaviour, T>, "T must derive from Behaviour");

	std::cout << "Behaviour::subscribe called" << std::endl;
	std::cout << "  cachedWorld=" << (cachedWorld ? "valid" : "null") << std::endl;

	if (!cachedWorld)
	{
		std::cout << "  FAILED: cachedWorld is null" << std::endl;
		return;
	}

	if (!dispatcher)
	{
		std::cout << "  FAILED: dispatcher is null" << std::endl;
		return;
	}

	std::cout << "  Subscribing to event..." << std::endl;

	T* self = static_cast<T*>(this);
	subscriptions->subscribe<EventType>(*dispatcher,
		[self, method](const EventType& e)
		{
			std::cout << "  Event callback triggered!" << std::endl;
			(self->*method)(e);
		});

	std::cout << "  Subscription successful, count=" << subscriptions->size() << std::endl;
}