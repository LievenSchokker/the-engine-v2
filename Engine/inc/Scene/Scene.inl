#pragma once


#include "SlotMap/Slot.h"
#include "GameObject/GameObject.h"


template<typename Func>
void Scene::forEachGameObject(Func&& func) {
	gameObjects.forEach(std::forward<Func>(func));
}

template<typename Func>
void Scene::forEachGameObject(Func&& func) const {
	gameObjects.forEach(std::forward<Func>(func));
}

template<typename Component>
std::vector<Component*> Scene::getAllComponentsOfType() const {
	std::vector<Component*> result;

	forEachGameObject([&](const GameObject& obj) {
		if (!obj.getIsActive()) return;

		for (Component* component : obj.getComponents<Component>()) {
			result.push_back(component);
		}
	});

	return result;
}

