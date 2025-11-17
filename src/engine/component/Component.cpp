//
// Created by samle on 10/11/2025.
//

#include "../../../inc/engine/component/Component.h"
#include "../../../inc/engine/component/Transform.h"
#include "../../../inc/engine/game_object/GameObject.h"


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
