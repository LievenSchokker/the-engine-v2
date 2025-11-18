//
// Created by samle on 10/11/2025.
//

#include "GameObject/GameObject.h"

#include "Behaviour/Behaviour.h"
#include "Component/ComponentManager.h"
#include "Component/Transform.h"


GameObject::GameObject()
{
    componentManager = std::make_unique<ComponentManager>(this);
    transform = std::make_unique<Transform>();

    name = "GameObject";
    layer = 0;
    tag = "";
    isActive = true;
    isStatic = false;
}


GameObject::~GameObject()
{
    componentManager->removeAllComponents();
    transform = nullptr;
}


bool GameObject::compareTag(const std::string& other)
{
    return tag == other;
}


bool GameObject::hasComponent(Component* comp) const
{
    return componentManager->hasComponent(comp);
}


void GameObject::removeComponent(Component* comp)
{
    componentManager->removeComponent(comp);
}


 std::vector<Behaviour*> GameObject::getActiveBehaviours() const
{
    std::vector<Behaviour*> activeBehaviours;

    for (Behaviour* behaviour: componentManager->getAllBehaviours())
    {
        if (behaviour == nullptr)
            continue;

        if (behaviour->getIsEnabled())
            activeBehaviours.push_back(behaviour);
    }

    return activeBehaviours;
}


ComponentManager* GameObject::getComponentManager() const
{
    return componentManager.get();
}


Transform* GameObject::getTransform() const
{
    return transform.get();
}


std::string GameObject::getName() const
{
    return name;
}


int GameObject::getLayer() const
{
    return layer;
}


std::string GameObject::getTag() const
{
    return tag;
}


bool GameObject::getIsActive() const
{
    return isActive;
}


bool GameObject::getIsStatic() const
{
    return isStatic;
}


int GameObject::getComponentCount() const
{
    return componentManager->getComponentCount();
}


void GameObject::setName(const std::string& newName)
{
    name = std::move(newName);
}


void GameObject::setLayer(int newLayer)
{
    layer = newLayer;
}


void GameObject::setTag(const std::string& newTag)
{
    tag = std::move(newTag);
}


void GameObject::setIsActive(bool value)
{
    isActive = value;
}


void GameObject::setIsStatic(bool value)
{
    isStatic = value;
}








