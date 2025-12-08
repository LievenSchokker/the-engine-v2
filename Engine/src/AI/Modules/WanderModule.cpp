//
// Created by samle on 05/12/2025.
//

#include "AI/Modules/WanderModule.h"
#include "Component/Transform.h"
#include "Math/Vector2.h"
#include <random>


void WanderModule::initialise()
{
    std::random_device device;
    randomEngine = std::default_random_engine(device());
}


float WanderModule::getRandomBetween(float min, float max)
{
    std::uniform_real_distribution<float> randomDistribution (min, max);
    return randomDistribution(randomEngine);
}


Vector2 WanderModule::compute()
{
    /// Compute a displacement on the current target
    Vector2 randomDisplacement = Vector2{
        getRandomBetween(-1, 1) * areaJitter,
        getRandomBetween(-1, 1) * areaJitter };

    currentTarget += randomDisplacement;

    /// Project the currentTarget onto the radius (normalise, then scale up to radius)
    currentTarget.normalize();
    currentTarget *= areaRadius;

    /// Compute a new target vector, projected in front of the agent
    Vector2 target = currentTarget + agentTransform.forward() * areaDistance;

    /// Return direction vector towards the target.
    return target.normalised();
}


float WanderModule::getAreaDistance() const
{
    return areaDistance;
}

float WanderModule::getAreaRadius() const
{
    return areaRadius;
}

float WanderModule::getAreaJitter() const
{
    return areaJitter;
}


void WanderModule::setAreaDistance(float value)
{
    areaDistance = value;
}


void WanderModule::setAreaRadius(float value)
{
    areaRadius = value;
}


void WanderModule::setAreaJitter(float value)
{
    areaJitter = value;
}