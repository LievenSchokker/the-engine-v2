#pragma once

#include "Component/ComponentManager.h"

#include <algorithm>
#include <type_traits>

template <typename T, typename... Args>
T* ComponentManager::addComponent(Args&&... args)
{
	static_assert(std::is_base_of_v<Component, T>,
	              "T must derive from Component!");

	if (gameObject == nullptr) return nullptr;

	if constexpr (std::is_same_v<Transform, T>)
	{
		return nullptr;
	}

	auto iterator = getComponentIterator<T>();
	if (iterator != components.end())
	{
		return dynamic_cast<T*>(iterator->get());
	}

	auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
	T* rawPtr = newComponent.get();

	newComponent->setGameObject(gameObject);

	if constexpr (std::is_base_of_v<Behaviour, T>)
	{
		behaviours.push_back(static_cast<Behaviour*>(rawPtr));
	}

	components.push_back(std::move(newComponent));
	return rawPtr;
}

template <typename T>
T* ComponentManager::getComponent() const
{
	if (gameObject == nullptr) return nullptr;

	if constexpr (std::is_same_v<Transform, T>)
	{
		return gameObject->getTransform();
	}

	auto iterator = getComponentIterator<T>();
	if (iterator != components.end())
	{
		return dynamic_cast<T*>(iterator->get());
	}
	else return nullptr;
}


template <typename T>
bool ComponentManager::tryGetComponent(T*& out) const
{
	T* component = getComponent<T>();
	if (component == nullptr) return false;
	else
	{
		out = component;
		return true;
	}
}


template <typename T>
void ComponentManager::removeComponent()
{
	if (gameObject == nullptr) return;
	auto iterator = getComponentIterator<T>();
	if (iterator != components.end())
	{
		components.erase(iterator);
	}
}

template <typename T>
bool ComponentManager::hasComponent() const
{
	for (const auto& c : components)
	{
		if (dynamic_cast<T*>(c.get()) != nullptr) return true;
	}

	return false;
}

template <typename T>
auto ComponentManager::getComponentIterator() -> std::vector<std::unique_ptr<
	Component>>::iterator
{
	return std::find_if(components.begin(), components.end(),
	                    [](const std::unique_ptr<Component>& comp)
	                    {
		                    return dynamic_cast<T*>(comp.get()) != nullptr;
	                    });
}


template <typename T>
auto ComponentManager::getComponentIterator() const -> std::vector<
	std::unique_ptr<Component>>::const_iterator
{
	return std::find_if(components.cbegin(), components.cend(),
	                    [](const std::unique_ptr<Component>& comp)
	                    {
		                    return dynamic_cast<T*>(comp.get()) != nullptr;
	                    });
}

template <typename T>
std::vector<T*> ComponentManager::getAllComponentsOfType() const
{
	std::vector<T*> result;
	for ( const auto& comp : components )
	{
		T* casted = dynamic_cast<T*>(comp.get());
		if ( casted != nullptr )
		{
			result.push_back(casted);
		}
	}
	return result;
}
