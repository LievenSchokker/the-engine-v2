//
// Created by samle on 02/12/2025.
//

#include "AI/Agent.h"

#include "Component/Transform.h"

void Agent::onAwake()
{
    transform = gameObject->getTransform();

    if (transform == nullptr)
    {
        setEnabled(false);
    }
}


void Agent::update()
{
    Vector2 velocity = computeDesiredVelocity();

    // transform->setPosition(transform->getPosition() + velocity);
}

Vector2 Agent::computeDesiredVelocity()
{
    return Vector2::zero();
}

Vector2 Agent::computeModuleForce()
{
    return Vector2{0,0};
}

float Agent::getMaxModuleForceMagnitude() const
{
    return maxModuleForceMagnitude;
}

void Agent::setMaxModuleForceMagnitude(float max)
{
    maxModuleForceMagnitude = max;
}

float Agent::getMaxVelocityMagnitude() const
{
    return maxVelocityMagnitude;
}

void Agent::setMaxVelocityMagnitude(float max)
{
    maxVelocityMagnitude = max;
}





