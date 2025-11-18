//
// Created by samle on 10/11/2025.
//
#pragma once


#include "GameObject/GameObject.h"

template<typename T>
T* Component::addComponent()
{
    if (gameObject == nullptr)
        return nullptr;

    return gameObject->template addComponent<T>();
}


template<typename T>
T* Component::getComponent() const
{
    if (gameObject == nullptr)
        return nullptr;

    return gameObject->template getComponent<T>();
}


template<typename T>
bool Component::tryGetComponent(T*& out) const
{
    if (gameObject == nullptr)
        return false;

    return gameObject->template tryGetComponent<T>(out);
}
