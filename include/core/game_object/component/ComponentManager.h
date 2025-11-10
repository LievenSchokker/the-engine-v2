//
// Created by samle on 10/11/2025.
//

#pragma once

#include <vector>
#include <memory>

#include "Component.h"
#include "../behaviour/behaviour.h"
#include "../GameObject.h"

class ComponentManager
{
    ComponentManager() = default;
    ~ComponentManager() = default;

    /// Functions:
    template <typename T>
    void addComponent();

    template <typename T>
    T* getComponent() const;

    template <typename T>
    void removeComponent();

    void removeAllComponents();

    std::Vector<Behaviour&> getAllBehaviours();

     void activateAll();
     void deactivateAll();

    /// Variables:
    public:
        std::vector<std::unique_ptr<Component>> components;
        GameObject* const owner;
}

/// Template implementations:
#include "ComponentManagerImplementation.h"