//
// Created by samle on 10/11/2025.
//

#include "Component/BaseComponentTypes/Component.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"

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

ComponentType Component::getComponentType() const
{
	return ComponentType::Unknown;
}
void Component::deserialize(CerealReadArchive& archive){}
void Component::serialize(CerealWriteArchive& archive) const{}

