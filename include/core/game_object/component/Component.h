//
// Created by samle on 10/11/2025.
//

#pragma once

#include "Transform.h"
#include "../GameObject.h"

class Component
{
    /// Functions:
    public:
        Component() = default;
        virtual ~Component() = 0;

        template <typename T>
        T* getComponent();

        template <typename T>
        bool tryGetComponent(T*& out);

    /// Variables:
        GameObject* owner = nullptr;
        Transform* transform = nullptr;
};

/// Implementation of the template functions:
#include "ComponentImplementation.h"