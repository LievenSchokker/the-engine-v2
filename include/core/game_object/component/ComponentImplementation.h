//
// Created by samle on 10/11/2025.
//

#pragma once

#include "Component.h"

template <typename T>
T* Component::GetComponent()
{
    if (!owner)
        return nullptr;

    for (Component* comp : owner->components)
    {
        T* casted = dynamic_cast<T*>(comp);
        if (casted) return casted;
    }

    return nullptr;
}