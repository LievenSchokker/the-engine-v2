//
// Created by samle on 10/11/2025.
//


#pragma once


#include <vector>
#include <memory>

class Component;
class GameObject;
class Behaviour;

class ComponentManager
{
public:
    ComponentManager();
    ~ComponentManager();

    /// Functions:
    template <typename T>
    T* addComponent();

    template <typename T>
    T* getComponent() const;

    template <typename T>
    bool tryGetComponent(T*& out) const;

    template <typename T>
    void removeComponent();

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
    GameObject* const owner;
};

/// Template implementations:
#include "ComponentManagerImplementation.h"
