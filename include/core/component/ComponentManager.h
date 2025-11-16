//
// Created by samle on 10/11/2025.
//


#pragma once

class Component;
class GameObject;
class Behaviour;
class Transform;

#include <vector>
#include <memory>


class ComponentManager
{
public:
    ComponentManager(GameObject* parent_)
        : gameObject(parent_){}

    ~ComponentManager();

    template <typename T>
    T* addComponent();

    template <typename T>
    T* getComponent() const;

    template <typename T>
    bool tryGetComponent(T*& out) const;

    template <typename T>
    void removeComponent();

    template <typename T> bool hasComponent() const;

    bool hasComponent(Component* comp) const;
    void removeComponent(Component* component);

    void removeAllComponents();

    std::vector<Behaviour*> getAllBehaviours();

    void activateAll();

    void deactivateAll();

    int getComponentCount() const;

private:
    template <typename T>
    std::vector<std::unique_ptr<Component>>::iterator getComponentIterator();
    template <typename T>
    std::vector<std::unique_ptr<Component>>::const_iterator getComponentIterator() const;

private:
    GameObject* const gameObject;
    std::vector<std::unique_ptr<Component>> components;
};

/// Template implementations:
#include "ComponentManagerImplementation.inl"
