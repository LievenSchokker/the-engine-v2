//
// Created by samle on 10/11/2025.
//

#include "../../../include/core/component/ComponentManager.h"
#include "../../../include/core/game_object/GameObject.h"
#include "../../../include/core/component/Component.h"


ComponentManager::~ComponentManager()
{
    removeAllComponents();
}


std::vector<Behaviour*> ComponentManager::getAllBehaviours()
{
    return {};
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




