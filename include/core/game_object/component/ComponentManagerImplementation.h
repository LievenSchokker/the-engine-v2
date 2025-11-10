//
// Created by samle on 10/11/2025.
//
#pragma once
#include "ComponentManager.h"
#include <algorithm>



template<typename T>
void ComponentManager::addComponent()
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

    if (owner == nullptr)
        return;

    auto iterator = getComponentIterator<T>();
    if (iterator != components.end())
    {
        /// Todo: 1) Implement ways to allow certain components to be duplicates, 2) Thow warning, exception, nothing here?
        return;
    }
    else
    {
        std::unique_ptr<Component> component = std::make_unique<T>();
        component->setOwner(owner);
        components.push_back(std::move(component));
    }
}


template<typename T>
T *ComponentManager::getComponent() const
{
    if (owner == nullptr)
        return nullptr;
    auto iterator = getComponentIterator<T>();
    if (iterator != components.end())
    {
        return dynamic_cast<T *>(iterator->get());
    }
    else
        return nullptr;
}


template<typename T>
bool ComponentManager::tryGetComponent(T *&out) const
{
    T *component = getComponent<T>();
    if (component == nullptr)
        return false;
    else
    {
        out = component;
        return true;
    }
}


template<typename T>
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

template<typename T>
std::vector<std::unique_ptr<Component> >::iterator ComponentManager::getComponentIterator()
{
    return std::find_if(components.begin(), components.end(),
                        [](const std::unique_ptr<Component> &comp)
                        {
                            return dynamic_cast<T *>(comp.get()) != nullptr;
                        });
}