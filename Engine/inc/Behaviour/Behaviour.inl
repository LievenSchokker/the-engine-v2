#pragma once


#include "Behaviour.h"
#include "Scene/SceneManager.h"

#include <iostream>


/// Template method that captures everything in a lambda.
/// This is safe because behaviour is managed by componentManager which use's uniqe_ptr
template <typename EventType, typename T>
void Behaviour::subscribe(void (T::*method)(const EventType&))
{
	static_assert(std::is_base_of_v<Behaviour, T>, "T must derive from Behaviour");
	T* self = static_cast<T*>(this);

	subscriptions.subscribe<EventType>(
		[self, method](const EventType& e)
		{
			std::cout << typeid(e).name() << std::endl;
			(self->*method)(e);
		});
}

template <typename EventType, typename T>
void Behaviour::subscribe(void (T::*method)(const EventType&) const)
{
	static_assert(std::is_base_of_v<Behaviour, T>, "T must derive from Behaviour");

	const T* self = static_cast<const T*>(this);

	subscriptions.subscribe<EventType>(
		[self, method](const EventType& e)
		{
			(self->*method)(e);
		});
}