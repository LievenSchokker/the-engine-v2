//
// Created by samle on 10/11/2025.
//
#pragma once

#include "../component/Component.h"

template<typename T>
T* Component::getComponent() const
{
    if (owner == nullptr)
        return nullptr;

    T* component = owner->template getComponent<T>();

    if (component != nullptr)
        return component;
    else
        throw std::runtime_error("GameObject does not have a component of type T!");
}

template<typename T>
bool Component::tryGetComponent(T*& out) const
{
    if (owner == nullptr)
        return false;

    out = owner->template tryGetComponent<T>();
    return (out != nullptr);
}
