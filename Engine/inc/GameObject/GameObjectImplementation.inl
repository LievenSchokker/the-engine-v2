#pragma once


#include "Component/ComponentManager.h"


template <typename T, typename... Args>
T* GameObject::addComponent(Args&&... args)
{
	return componentManager->template addComponent<T>(
		std::forward<Args>(args)...);
}

template <typename T>
T* GameObject::getComponent() const
{
	return componentManager->template getComponent<T>();
}


template <typename T>
T* GameObject::getOrAddComponent()
{
	T* component = componentManager->template getComponent<T>();

	if (component == nullptr)
	{
		component = componentManager->template addComponent<T>();
	}

	return component;
}


template <typename T>
bool GameObject::tryGetComponent(T*& out) const
{
	return componentManager->template tryGetComponent<T>(out);
}


template <typename T>
bool GameObject::hasComponent() const
{
	return componentManager->template hasComponent<T>();
}

template <typename T>
void GameObject::removeComponent()
{
	componentManager->template removeComponent<T>();
}