//
// Created by samle on 10/11/2025.
//

#include "Component/ComponentManager.h"
#include "GameObject/GameObject.h"
#include "Component/Component.h"
#include "Behaviour/Behaviour.h"


ComponentManager::~ComponentManager()
{
    destroyAllComponents();
}


const std::vector<Behaviour*>& ComponentManager::getAllBehaviours() const
{
    return behaviours;
}


void ComponentManager::enableAllBehaviours()
{
    for (auto& behaviour : behaviours)
    {
        behaviour->setEnabled(true);
    }
}


void ComponentManager::disableAllBehaviours()
{
    // for (auto& behaviour : behaviours)
    // {
    //     behaviour->setEnabled(false);
    // }
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


void ComponentManager::destroyAllComponents()
{
    for (auto& comp : components)
    {
        if (comp != nullptr)
        {
            comp->onDestroy();
        }
    }

    /// clear() deletes the components internally, because they are stored as unique_ptr inside the components vector.
    components.clear();
}


size_t ComponentManager::getComponentCount() const
{
    return components.size();
}


