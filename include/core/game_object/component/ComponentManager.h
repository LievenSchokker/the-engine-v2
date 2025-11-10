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
    public:
        ComponentManager();

        ~ComponentManager() = default;

        /// Functions:
        template<typename T>
        void addComponent();

        template<typename T>
        T *getComponent() const;

        template<typename T>
        bool tryGetComponent(T *&out) const;

        template<typename T>
        void removeComponent();

        void removeAllComponents();

        std::vector<Behaviour &> getAllBehaviours();

        void activateAll();

        void deactivateAll();

    private:
        template<typename T>
        std::vector<std::unique_ptr<Component>>::iterator getComponentIterator();

        /// Variables:
    public:
        std::vector<std::unique_ptr<Component> > components;
        GameObject *const owner;
};

/// Template implementations:
#include "ComponentManagerImplementation.h"
