#pragma once


#include "Scene.h"
#include "GameObject/GameObject.h"
#include <typeindex>

template <typename T>
std::vector<T*> Scene::getAllComponentsOfType() const
{
	std::vector<T*> result;
	result.reserve(gameObjects.size());

	for (const auto& gameObject : gameObjects)
	{
		if (!gameObject->getIsActive())
			continue;

		T* component = gameObject->getComponent<T>();
		if (component != nullptr)
		{
			result.push_back(component);
		}
	}

	return result;
}