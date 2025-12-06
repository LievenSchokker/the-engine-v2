//
// Created by samle on 05/12/2025.
//

#include "../../../inc/AI/Modules/WanderModule.h"
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
    circleRadius = value;
}


void WanderModule::setAreaJitter(float value)
{
    jitter = value;
}