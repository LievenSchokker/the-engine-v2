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
    Vector2 randomDisplacement = Vector2{
        getRandomBetween(-1, 1) * areaJitter,
        getRandomBetween(-1, 1) * areaJitter };

    currentTarget += randomDisplacement;

    currentTarget.normalize();
    currentTarget *= areaRadius;

    Vector2 target = currentTarget + agentTransform.forward() * areaDistance;

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