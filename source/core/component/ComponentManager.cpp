//
// Created by samle on 10/11/2025.
//

#include "../../../include/core/component/ComponentManager.h"

/// TODO: Implement functions once behaviour class is implemented.

ComponentManager::ComponentManager()
{

}

ComponentManager::~ComponentManager()
{
    removeAllComponents();
}


std::vector<Behaviour*> ComponentManager::getAllBehaviours()
{

}

void ComponentManager::activateAll()
{

}

void ComponentManager::deactivateAll()
{

}

void ComponentManager::removeAllComponents()
{
    components.clear();
}




