//
// Created by samle on 11/11/2025.
//

#pragma once

#include "../component/ComponentManager.h"

template<typename T>
void GameObject::addComponent()
{
    componentManager->addComponent<T>();
}

template<typename T>
T* GameObject::getComponent()
{
   return  componentManager->getComponent<T>();
}

template<typename T>
T* GameObject::getOrAddComponent()
{
    T *component = componentManager->getComponent<T>();

    if (component == nullptr)
    {
       component = componentManager->addComponent<T>();
    }

    return component;
}

template<typename T>
bool GameObject::tryGetComponent()
{
    return componentManager->tryGetComponent<T>();
}

template <typename T>
void GameObject::removeComponent()
{
    componentManager->removeComponent<T>();
}

