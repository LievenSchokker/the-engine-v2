//
// Created by samle on 10/11/2025.
//
#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "Component/ComponentManager.h"
#include "Component/Transform.h"
#include "GameObject/ScenePlaceholder.h"

GameObject::GameObject()
{
    componentManager = std::make_unique<ComponentManager>(this);
    transform = std::make_unique<Transform>();
    name = "GameObject";
    layer = 0;
    tag = "";
    isActive = true;
    isStatic = false;
    isDestroyed = false;

    /// TODO: Remove this line:
    scene = nullptr;
}


GameObject::~GameObject()
{
    componentManager->destroyAllComponents();
    transform = nullptr;
}


bool GameObject::compareTag(const std::string& other)
{
    return tag == other;
}


bool GameObject::hasComponent(Component *comp) const
{
    return componentManager->hasComponent(comp);
}


void GameObject::removeComponent(Component *comp)
{
    componentManager->removeComponent(comp);
}


const std::vector<Behaviour*>& GameObject::getAllBehaviours() const
{
    return componentManager->getAllBehaviours();
}


const std::vector<Behaviour*>& GameObject::getEnabledBehaviours() const
{
    return componentManager->getEnabledBehaviours();
}

ComponentManager* GameObject::getComponentManager() const
{
    return componentManager.get();
}


void GameObject::destroy()
{
    if (isDestroyed)
        return;

    isDestroyed = true;
    setActive(false);

    componentManager->disableAllBehaviours();

    if (scene != nullptr)
        scene->queueDestroy(this);
}


void GameObject::onSceneDestroy()
{
    componentManager->destroyAllComponents();
}


Transform *GameObject::getTransform() const
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


bool GameObject::getIsDestroyed() const
{
    return isDestroyed;
}


void GameObject::setName(const std::string &newName)
{
    name = std::move(newName);
}


void GameObject::setLayer(int newLayer)
{
    layer = newLayer;
}


void GameObject::setTag(const std::string &newTag)
{
    tag = std::move(newTag);
}


void GameObject::setActive(bool value)
{
    if (isActive == value)
        return;

    isActive = value;
}


void GameObject::setIsStatic(bool value)
{
    if (isStatic == value)
        return;

    isStatic = value;
}


void GameObject::setScene(Scene& newScene)
{
    scene = &newScene;
}


void GameObject::setBehavioursEnabled(const bool value) const
{
    if (value)
    {
        componentManager->enableAllBehaviours();
    }
    else
    {
        componentManager->disableAllBehaviours();
    }
}