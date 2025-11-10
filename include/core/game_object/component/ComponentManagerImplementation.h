//
// Created by samle on 10/11/2025.
//

#pragma once

#include "ComponentManager.h"

template <typename T>
void ComponentManager::addComponent()
{
    if (owner == nullptr)
        return;

    if (components.contains(typeid(T).name()))
        return;


}

template <typename T>
T* ComponentManager::getComponent() const
{
    if (owner == nullptr)
        return nullptr;


}

template <typename T>
void ComponentManager::removeComponent()
{
    if (owner == nullptr)
        return nullptr;


}