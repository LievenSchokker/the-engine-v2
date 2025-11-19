//
// Created by samle on 10/11/2025.
//

#include "Component/ComponentManager.h"
#include "GameObject/GameObject.h"
#include "Component/Component.h"


ComponentManager::~ComponentManager()
{
    removeAllComponents();
}


std::vector<Behaviour*> ComponentManager::getAllBehaviours()
{
    // implement with behaviour.
    return {};
}


void ComponentManager::activateAll()
{
    // implement with behaviour.
}


void ComponentManager::deactivateAll()
{
    // implement with behaviour.

}


bool ComponentManager::hasComponent(Component* comp) const
{
    if (comp == nullptr)
        return false;

    auto it = std::find_if(components.begin(), components.end(),
                           [&](const std::unique_ptr<Component>& component){
                               return component.get() == comp;
                           });

    return it != components.end();
}


void ComponentManager::removeComponent(Component* comp)
{
    if (!comp || !hasComponent(comp))
        return;

    auto it = std::find_if(components.begin(), components.end(),
                           [&](const std::unique_ptr<Component>& component)
                           { return component.get() == comp; });

    if (it != components.end())
    {
        components.erase(it);
    }
}


void ComponentManager::removeAllComponents()
{
    components.clear();
}


int ComponentManager::getComponentCount() const
{
    return components.size();
}


