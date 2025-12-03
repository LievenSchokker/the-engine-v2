//
// Created by samle on 10/11/2025.
//

#include "Component/Component.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"


Component::~Component()
{
    gameObject = nullptr;
    transform = nullptr;
}


void Component::setGameObject(GameObject* object)
{
    gameObject = object;
    transform = gameObject->getTransform();
}


GameObject* Component::getGameObject() const
{
    return gameObject;
}


const Transform* Component::getTransform() const
{
    return transform;
}
