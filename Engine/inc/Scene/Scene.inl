#pragma once


#include "Scene.h"
#include "GameObject/GameObject.h"

template <typename T>
std::vector<T*> Scene::getAllComponentsOfType() const
{
	std::vector<T*> result;

	for (const auto& gameObject : gameObjects)
	{
		if (!gameObject->getIsActive()) continue;

		auto components = gameObject->getAllComponentsOfType<T>();
		result.insert(result.end(), components.begin(), components.end());
	}

	return result;
}