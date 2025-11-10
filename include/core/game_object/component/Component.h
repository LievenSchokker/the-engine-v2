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
        Component();
        virtual ~Component() = 0;

        template <typename T>
        T* GetComponent();

        template <typename T>
        bool TryGetComponent(T*& out);

    /// Variables:
    public:
        GameObject* gameObject;
        Transform* transform;
}