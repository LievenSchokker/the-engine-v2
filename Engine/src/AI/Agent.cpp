//
// Created by samle on 02/12/2025.
//


#include "AI/Agent.h"
#include "Component/Transform.h"


void Agent::onAwake()
{
    if (transform == nullptr)
    {
        setEnabled(false);
    }
}


void Agent::update(float deltaTime, GameWorld* gameWorld)
{
    Vector2 velocity = computeDesiredVelocity();

    transform->rotateTowards(velocity, rotationTurnRate, deltaTime);
    transform->moveTowards(transform->getPosition() + velocity, velocity.magnitude() * deltaTime);
}


Vector2 Agent::computeDesiredVelocity()
{
    Vector2 desiredForce = computeModuleForce();

    if (desiredForce.magnitude() > maxSpeed)
    {
        desiredForce = desiredForce.normalised() * maxSpeed;
    }

    return desiredForce;
}


Vector2 Agent::computeModuleForce()
{
    Vector2 totalForce = Vector2::zero();

    for (std::unique_ptr<ModuleData>& moduleData : moduleDatas)
    {
        /// Skip inactive modules
        if (!moduleData->isModuleActive())
            continue;

        float weight = moduleData->getWeight();

        /// Skip computation for 0 weights.
        if (weight <= 0)
            continue;

        Vector2 direction = moduleData->getModule()->compute();
        direction.normalize();
        totalForce += direction * weight;
    }


    return totalForce;
}


float Agent::getMaxSpeed() const
{
    return maxSpeed;
}


void Agent::setMaxSpeed(float max)
{
    maxSpeed = max;
}


size_t Agent::getModuleCount() const
{
    return moduleDatas.size();
}


float Agent::getArrivingDistance() const
{
    return arrivingDistance;
}


void Agent::setArrivingDistance(float distance)
{
    arrivingDistance = distance;
}


float Agent::getRotationTurnRate() const
{
    return rotationTurnRate;
}

void Agent::setRotationTurnRate(float value)
{
    rotationTurnRate = value;
}








