#pragma once


#include "Slot.h"
#include "GameObject/GameObject.h"


template<typename Function>
void Scene::forEachGameObject(Function&& func)
{
    for (auto& slot : slots) {
        if (slot.object) {
            func(*slot.object);
        }
    }
}

template<typename Function>
void Scene::forEachGameObject(Function&& func) const
{
    for (const auto& [object, generation] : slots) {
        if (object) {
            func(*object);
        }
    }
}

template <typename Component>
std::vector<Component*> Scene::getAllComponentsOfType() const
{
    std::vector<Component*> result;

    forEachGameObject([&](const GameObject& obj) {
        if (!obj.getIsActive()) return;

        for (Component* component : obj.getComponents<Component>()) {
            result.push_back(component);
        }
    });

    return result;
}