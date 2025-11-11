//
// Created by samle on 10/11/2025.
//

#include "../../../include/core/component/Component.h"

Component::~Component()
{
    owner = nullptr;
    transform = nullptr;
}

void Component::setOwner(GameObject* object)
{
    owner = object;
    transform = object->getTransform();
}
