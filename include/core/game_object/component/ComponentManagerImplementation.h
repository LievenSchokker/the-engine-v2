//
// Created by samle on 10/11/2025.
//

#pragma once

#include "ComponentManager.h"
#include <algorithm>

template <typename T>
void ComponentManager::addComponent()
{
    if (owner == nullptr)
        return;

    auto iterator = std::find_if(components.begin(), components.end(),
        [](const std::unique_ptr<Component>& comp) {
            return dynamic_cast<T*>(comp.get()) != nullptr;
        });

    if (iterator != components.end())
    {
        /// Todo: 1) No duplicates allowed? 2) Thow warning, exception, nothing?
        #warning "Cannot add a duplicate component to GameObject!"
        return;
    }

    components.push_back(std::make_unique<T>());
}

template <typename T>
std::vector<std::unique_ptr<Component>>::iterator ComponentManager::getComponentIterator()
{
    return std::find_if(components.begin(), components.end(),
        [](const std::unique_ptr<Component>& comp) {
            return dynamic_cast<T*>(comp.get()) != nullptr;
        });
}

template <typename T>
T* ComponentManager::getComponent() const
{
    if (owner == nullptr)
        return nullptr;

    auto iterator = getComponentIterator<T>();
    if (iterator != components.end())
    {
        return dynamic_cast<T*>(iterator->get());
    }
    else
        return nullptr;
}

template<typename T>
bool ComponentManager::tryGetComponent(T*& out) const
{
    T* component = getComponent<T>();

    if (component == nullptr)
        return false;
    else
    {
        out = component;
        return true;
    }
}


template <typename T>
void ComponentManager::removeComponent()
{
    if (owner == nullptr)
        return;

    auto iterator = getComponentIterator<T>();
    if (iterator != components.end())
    {
        components.erase(iterator);
    }
}