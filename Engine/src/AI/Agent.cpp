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

    transform->setPosition(transform->getPosition() + velocity);
}

Vector2 Agent::computeDesiredVelocity()
{
    return Vector2::zero();
}

Vector2 Agent::computeModuleForce()
{
    Vector2 totalForce = Vector2::zero();
    Vector2 accumulatedForce = Vector2::zero();

    for (std::unique_ptr<BaseAgentModule>& modulePtr : modules)
    {
        BaseAgentModule* module = modulePtr.get();
        float weight = moduleWeights[module->getModuleType()];
        Vector2 force = module->compute();

        accumulatedForce += force * weight;
    }

    if (accumulatedForce.magnitude() > maxModuleForceMagnitude)
    {
        accumulatedForce.normalize();
        accumulatedForce *= maxModuleForceMagnitude;
    }

    return totalForce;
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





