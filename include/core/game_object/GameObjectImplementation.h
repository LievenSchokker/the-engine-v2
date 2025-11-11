//
// Created by samle on 11/11/2025.
//


#pragma once


#include "GameObject.h"


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

template<typename T> T* GameObject::getOrAddComponent()
{
    T *component = componentManager->getComponent<T>();
    if (component == nullptr)
    {
        componentManager->addComponent<T>();
        component = componentManager->getComponent<T>();
    }
    return component;
}

template<typename T> bool GameObject::tryGetComponent()
{
    return componentManager->tryGetComponent<T>();
}
