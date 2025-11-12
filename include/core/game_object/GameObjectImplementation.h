//
// Created by samle on 11/11/2025.
//

#pragma once

#include "../component/ComponentManager.h"


template<typename T>
void GameObject::addComponent()
{
    componentManager->template addComponent<T>();
}

template<typename T>
T* GameObject::getComponent()
{
   return  componentManager->template getComponent<T>();
}

template<typename T>
T* GameObject::getOrAddComponent()
{
    T *component = componentManager->template getComponent<T>();

    if (component == nullptr)
    {
       component = componentManager->template addComponent<T>();
    }

    return component;
}

template<typename T>
bool GameObject::tryGetComponent()
{
    return componentManager->template tryGetComponent<T>();
}

template <typename T>
void GameObject::removeComponent()
{
    componentManager->template removeComponent<T>();
}

