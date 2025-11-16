//
// Created by samle on 10/11/2025.
//

#pragma once

#include <algorithm>
#include <type_traits>


template<typename T>
T* ComponentManager::addComponent()
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component!");

    if (parent == nullptr)
        return nullptr;

    if constexpr (std::is_same_v<Transform, T>) {
        return nullptr;
    }


    auto iterator = getComponentIterator<T>();
    if (iterator != components.end())
    {
        /// Todo: 1) Implement ways to allow certain components to be duplicates, 2) Thow warning, exception, nothing here?
        return dynamic_cast<T*>(iterator->get());
    }

    auto& component = components.emplace_back(std::make_unique<T>());
    component->setGameObject(parent);

    return dynamic_cast<T*>(component.get());
}


template<typename T>
T *ComponentManager::getComponent()
{
    if (parent == nullptr)
        return nullptr;

    if constexpr (std::is_same_v<Transform, T>) {
        return parent->getTransform();
    }

    auto iterator = getComponentIterator<T>();
    if (iterator != components.end())
    {
        return dynamic_cast<T *>(iterator->get());
    }
    else
        return nullptr;
}


template<typename T>
bool ComponentManager::tryGetComponent(T *&out)
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
    if (parent == nullptr)
        return;
    auto iterator = getComponentIterator<T>();
    if (iterator != components.end())
    {
        components.erase(iterator);
    }
}

template<typename T>
bool ComponentManager::hasComponent()
{
    for (const auto& c : components)
    {
        if (dynamic_cast<T*>(c.get()) != nullptr)
            return true;
    }

    return false;
}

template<typename T>
auto ComponentManager::getComponentIterator() -> std::vector<std::unique_ptr<Component> >::iterator
{
    return std::find_if(components.begin(), components.end(),
                        [](const std::unique_ptr<Component> &comp)
                        {
                            return dynamic_cast<T *>(comp.get()) != nullptr;
                        });
}


template<typename T>
auto ComponentManager::getComponentIterator() const -> std::vector<std::unique_ptr<Component>>::const_iterator
{
    return std::find_if(components.cbegin(), components.cend(),
                        [](const std::unique_ptr<Component>& comp)
                        {
                            return dynamic_cast<T*>(comp.get()) != nullptr;
                        });
}