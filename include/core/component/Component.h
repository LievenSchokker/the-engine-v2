//
// Created by samle on 10/11/2025.
//

#pragma once

class Transform;
class GameObject;

class Component
{
    /// Functions:
    public:
        Component() = default;
        virtual ~Component() = 0;

        void setGameObject(GameObject* object);

        template <typename T>
        T* getComponent() const;

        template <typename T>
        bool tryGetComponent(T*& out) const;


    /// Variables:
        GameObject* owner = nullptr;
        Transform* transform = nullptr;
};

/// Implementation of the template functions:
#include "ComponentImplementation.inl"
