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
    Vector2 totalModulesForce = computeModuleForce();

    if (totalModulesForce.magnitude() > maxVelocityMagnitude)
    {
        totalModulesForce = totalModulesForce.normalised() * maxVelocityMagnitude;
    }

    return totalModulesForce;
}

Vector2 Agent::computeModuleForce()
{
    Vector2 totalForce = Vector2::zero();
    Vector2 accumulatedForce = Vector2::zero();

    /// TODO: Add a data class that caches the Module and its weight instead of lookups every frame.
    // for (std::unique_ptr<BaseAgentModule>& modulePtr : modules)
    // {
    //     BaseAgentModule* module = modulePtr.get();
    //     float weight = moduleWeights[module->getModuleType()];
    //     Vector2 force = module->compute();
    //
    //
    //     accumulatedForce += force * weight;
    //
    //     /// Make sure the accumulated force vector doesnt exceed the max allowed magnitude.
    //     if (accumulatedForce.magnitude() > maxModuleForceMagnitude)
    //     {
    //         accumulatedForce = accumulatedForce.normalised() * maxModuleForceMagnitude;
    //     }
    //
    //     /// Add the current iterated module computation to the total force.
    //     totalForce += accumulatedForce;
    // }

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





