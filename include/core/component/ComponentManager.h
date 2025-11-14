//
// Created by samle on 10/11/2025.
//


#pragma once

#include <vector>
#include <memory>

class GameObject;
class Behaviour;
class Component;

class ComponentManager
{
public:
    ComponentManager(GameObject* parent_)
        : parent(parent_){}

    ~ComponentManager();

    template <typename T>
    T* addComponent();

    template <typename T>
    T* getComponent() const;

    template <typename T>
    bool tryGetComponent(T*& out) const;

    template <typename T>
    void removeComponent();

    bool hasComponent(Component* comp) const;
    void addComponent(Component* component);
    void removeComponent(Component* component);

    void removeAllComponents();

    std::vector<Behaviour*> getAllBehaviours();

    void activateAll();

    void deactivateAll();

private:
    template <typename T>
    std::vector<std::unique_ptr<Component>>::iterator getComponentIterator();

    template <typename T>
    std::vector<std::unique_ptr<Component>>::const_iterator getComponentIterator() const;

    /// Variables:
public:
    std::vector<std::unique_ptr<Component>> components;
    GameObject* const parent;
};

/// Template implementations:
#include "ComponentManagerImplementation.inl"
