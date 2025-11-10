//
// Created by samle on 10/11/2025.
//
#pragma once
#include "component/ComponentManager.h"

class GameObject
{
    public:
        GameObject();
        ~GameObject();

        Transform* getTransform() const { return transform.get(); }

        /// Fields:
        const std::unique_ptr<ComponentManager> componentManager = nullptr;
        std::unique_ptr<Transform> const transform = nullptr;
};
