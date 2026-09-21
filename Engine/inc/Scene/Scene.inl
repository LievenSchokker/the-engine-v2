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
std::vector<Component*> Scene::getAllComponentsOfType() const{

	auto it = componentCaches.find(std::type_index(typeid(Component)));
	if (it != componentCaches.end()) {
		return std::any_cast<std::vector<Component*>>(it->second);
	}

	std::vector<Component*> result;
	forEachGameObject([&](const GameObject& obj) {
		for (Component* component : obj.getComponents<Component>()) {
			result.push_back(component);
		}
	});

	componentCaches[std::type_index(typeid(Component))] = result;
	return result;
}

