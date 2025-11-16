//
// Created by samle on 10/11/2025.
//

#pragma once

class Transform;
class GameObject;

class Component
{
    public:
        Component() = default;
        virtual ~Component();

        void setGameObject(GameObject* object);

        template <typename T>
        T* addComponent();

        template <typename T>
        T* getComponent() const;

        template <typename T>
        bool tryGetComponent(T*& out) const;

    public:
        GameObject* getGameObject() const;
        const Transform* getTransform() const;

    private:
        GameObject* gameObject = nullptr;
        const Transform* transform = nullptr;
};

/// Implementation of the template functions:
#include "ComponentImplementation.inl"
