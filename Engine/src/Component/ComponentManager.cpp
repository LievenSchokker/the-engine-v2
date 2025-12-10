//
// Created by samle on 10/11/2025.
//

#include "Component/ComponentManager.h"
#include "GameObject/GameObject.h"
#include "../../inc/Component/BaseComponentTypes/Component.h"
#include "Behaviour/Behaviour.h"


ComponentManager::~ComponentManager()
{
    destroyAllComponents();
}


const std::vector<Behaviour*>& ComponentManager::getAllBehaviours() const
{
    return behaviours;
}


const std::vector<Behaviour*>& ComponentManager::getEnabledBehaviours()
{
    enabledBehaviours.clear();

    for (Behaviour *behaviour: behaviours)
    {
        if (behaviour != nullptr && behaviour->getIsEnabled())
            enabledBehaviours.push_back(behaviour);
    }

    return enabledBehaviours;
}


void ComponentManager::enableAllBehaviours() const
{
    for (auto& behaviour : behaviours)
    {
        behaviour->setEnabled(true);
    }
}


void ComponentManager::disableAllBehaviours() const
{
    for (auto& behaviour : behaviours)
    {
        behaviour->setEnabled(false);
    }
}


bool ComponentManager::hasComponent(const Component* comp) const
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

    /// If its a behaviour, we also need to remove it from the behaviours vectors:
    if (auto* behaviour = dynamic_cast<Behaviour*>(comp))
    {
        behaviour->setEnabled(false);

        behaviours.erase(
            std::remove(behaviours.begin(), behaviours.end(), behaviour),
            behaviours.end()
        );

        enabledBehaviours.erase(
            std::remove(enabledBehaviours.begin(), enabledBehaviours.end(), behaviour),
            enabledBehaviours.end()

        );
    }

    /// Remove the component from the components vector.
    auto it = std::find_if(components.begin(), components.end(),
                           [&](const std::unique_ptr<Component>& component)
                           { return component.get() == comp; });

    if (it != components.end())
    {
        it->get()->onDestroy();
        components.erase(it);
    }
}


void ComponentManager::destroyAllComponents()
{
    while (!components.empty())
    {
        removeComponent(components.back().get());
    }
}


size_t ComponentManager::getComponentCount() const
{
    return components.size();
}


