//
// Created by samle on 10/11/2025.
//
#pragma once
#include "Component.h"

template<typename T>
T *Component::getComponent()
{
    if (owner == nullptr)
        return nullptr;

    for (Component *comp: owner->componentManager->components)
    {
        if (T *casted = dynamic_cast<T *>(comp))
            return casted;
    }
    return nullptr;
}

template<typename T>
bool Component::tryGetComponent(T*& out)
{
    if (owner == nullptr)
        return false;

    for (Component *comp: owner->componentManager->components)
    {
        if (T *casted = dynamic_cast<T *>(comp))
        {
            out = casted;
            return true;
        }
    }
    return false;
}
