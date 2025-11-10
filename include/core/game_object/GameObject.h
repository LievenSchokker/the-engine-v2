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


    /// Fields:
    std::unique_ptr<ComponentManager> componentManager = nullptr;
};
