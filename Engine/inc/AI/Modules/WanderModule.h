//
// Created by samle on 05/12/2025.
//

#pragma once


#include <random>

#include "AI/Modules/BaseAgentModule.h"
#include "Math/Vector2.h"

class WanderModule final : public BaseAgentModule
{
    public:
    explicit WanderModule(const Agent& _agent) : BaseAgentModule(_agent) {}
    explicit WanderModule(const Agent& _agent, float areaDistance_, float areaRadius_, float areaJitter_)
        : BaseAgentModule(_agent), areaDistance(areaDistance_), areaRadius(areaRadius_), areaJitter(areaJitter_) {}

    ~WanderModule() override = default;

    void initialise() override;
    Vector2 compute() override;

    float getAreaDistance() const;
    float getAreaRadius() const;
    float getAreaJitter() const;

    void setAreaDistance(float value);
    void setAreaRadius(float value);
    void setAreaJitter(float value);

private:
    Vector2 currentTarget = Vector2::zero();
    float getRandomBetween(float min, float max);;

    float areaDistance = 0.0f;
    float areaRadius = 0.0f;
    float areaJitter = 0.0f;

    std::default_random_engine randomEngine;

};


