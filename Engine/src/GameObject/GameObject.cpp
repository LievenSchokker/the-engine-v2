//
// Created by samle on 10/11/2025.
//
#include "GameObject/GameObject.h"

#include "Game.h"
#include "Behaviour/Behaviour.h"
#include "Component/ComponentManager.h"
#include "Component/Transform.h"
#include "GameObject/ScenePlaceholder.h"
#include "Scene/Scene.h"

GameObject::GameObject()
{
    componentManager = std::make_unique<ComponentManager>(this);
    transform = std::make_unique<Transform>();
    transform->setGameObject(this);
    name = "GameObject";
    layer = 0;
    tag = "";
    isActive = true;
    isStatic = false;
    isDestroyed = false;
    scene = nullptr;
}

GameObject::GameObject(const std::string& goName)
{
    componentManager = std::make_unique<ComponentManager>(this);
    transform = std::make_unique<Transform>();
    name = goName;
    layer = 0;
    tag = "";
    isActive = true;
    isStatic = false;
    isDestroyed = false;
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

int GameObject::getSceneId() const
{
    if (scene == nullptr)
        return -1;

    return sceneId;
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
    sceneId = scene->getSceneId(*this);
}

Scene* GameObject::getScene() const
{
    return scene;
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