//
// Created by samle on 10/11/2025.
//

#pragma once

#include "Component.h"

template <typename T>
T* Component::GetComponent()
{
    if (owner == nullptr)
        return nullptr;


/// #TODO: Implement this function with ComponentManager instead.
    for (Component* comp : owner->components)
    {
        T* casted = dynamic_cast<T*>(comp);
        if (casted) return casted;
    }

    return nullptr;
}

template <typename T>
bool TryGetComponent(T*& out)
{
    out = dynamic_cast<T*>(component);

    return (out != nullptr);
}

