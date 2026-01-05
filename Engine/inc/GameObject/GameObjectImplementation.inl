#pragma once

#include "GameObject.h"


#include <algorithm>


template <typename T, typename... Args>
T* GameObject::addComponent(Args&&... args)
{
    static_assert(std::is_base_of_v<Component, T>,
                  "T must derive from Component!");


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

    newComponent->setGameObject(this);

    internalAddComponent(std::move(newComponent));
    return rawPtr;
}

template <typename T>
T* GameObject::getComponent() const
{
    if constexpr (std::is_same_v<Transform, T>)
    {
        return getTransform();
    }

    auto iterator = getComponentIterator<T>();
    if (iterator != components.end())
    {
        return dynamic_cast<T*>(iterator->get());
    }
    else return nullptr;
}

template <typename T>
std::vector<T*> GameObject::getComponents() const
{
    std::vector<T*> result;
    for (const auto& component : components) {
        if (T* casted = dynamic_cast<T*>(component.get())) {
            result.push_back(casted);
        }
    }
    return result;
}

template <typename T>
bool GameObject::tryGetComponent(T*& out) const
{
    T* component = getComponent<T>();
    if (component == nullptr) return false;
    else
    {
        out = component;
        return true;
    }
}

template <typename T, typename... Args>
T* GameObject::getOrAddComponent(Args&&... args)
{
    T* component = nullptr;

    if (tryGetComponent<T>(component))
    {
        return component;
    }
    else
    {
        component = addComponent<T>(std::forward<Args>(args)...);
    }
    return component;

}



template <typename T>
bool GameObject::hasComponent() const
{
    for (const auto& c : components)
    {
        if (dynamic_cast<T*>(c.get()) != nullptr) return true;
    }

    return false;
}

template <typename T>
void GameObject::removeComponent()
{
    auto iterator = getComponentIterator<T>();
    if (iterator != components.end())
    {
        components.erase(iterator);
    }
}


template <typename T>
auto GameObject::getComponentIterator() -> std::vector<std::unique_ptr<
    Component>>::iterator
{
    return std::find_if(components.begin(), components.end(),
                        [](const std::unique_ptr<Component>& comp)
                        {
                            return dynamic_cast<T*>(comp.get()) != nullptr;
                        });
}

template <typename T>
auto GameObject::getComponentIterator() const -> std::vector<
    std::unique_ptr<Component>>::const_iterator
{
    return std::find_if(components.cbegin(), components.cend(),
                        [](const std::unique_ptr<Component>& comp)
                        {
                            return dynamic_cast<T*>(comp.get()) != nullptr;
                        });
}

template <typename T>
std::vector<T*> GameObject::getAllComponentsOfType() const
{
    std::vector<T*> result;
    for (const auto& component : components)
    {
        if (auto* casted = dynamic_cast<T*>(component.get()))
        {
            result.push_back(casted);
        }
    }
    return result;
}
