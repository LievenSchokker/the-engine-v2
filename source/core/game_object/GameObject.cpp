//
// Created by samle on 10/11/2025.
//


#include "../../../include/core/game_object/GameObject.h"
#include "../../../include/core/component/Transform.h"

GameObject::GameObject()
{
    componentManager = std::make_unique<ComponentManager>();
    transform = componentManager->addComponent<Transform>();

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

bool GameObject::compareTag(std::string other)
{
    return tag == other;
}

std::vector<Behaviour*> GameObject::getActiveBehaviours() const
{
    return std::vector<Behaviour*>{};
}

ComponentManager* GameObject::getComponentManager() const
{
    return componentManager.get();
}

Transform* GameObject::getTransform() const
{
    return componentManager->getComponent<Transform>();
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

void GameObject::setName(std::string newName)
{
    name = newName;
}

void GameObject::setLayer(int newLayer)
{
    layer = newLayer;
}

void GameObject::setTag(std::string newTag)
{
    tag = newTag;
}

void GameObject::setIsActive(bool value)
{
    isActive = value;
}

void GameObject::setIsStatic(bool value)
{
    isStatic = value;
}








