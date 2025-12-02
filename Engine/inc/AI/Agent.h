//
// Created by samle on 02/12/2025.
//


#pragma once


#include "Behaviour/Behaviour.h"
#include <map>
#include <unordered_map>

#include "GameObject/Vector2.h"
struct Vector2;

class Agent final : public Behaviour
{
    public:
        Agent() :
            currentVelocity(Vector2{0,0}), maxMotionMagnitude(0), maxSpeed(0) {};
        ~Agent() override = default;

        void update() override;
        Vector2 computeModuleForce();
        Vector2 computeDesiredVelocity();

        bool addAgentModule(); /// Todo: add module param
        bool removeAgentModule(); /// Todo: add module param
        bool setModuleWeight(float desiredWeight); // Todo: add module param
        bool setModuleStatus(); /// Todo: add module param, add moduleStatus param

    private:
        std::vector<int> modulePositions; /// Todo: replace int with ModuleData
        std::unordered_map<int, float> moduleWeights; /// Todo: Replace int with moduleType
        Vector2 currentVelocity;
        float maxMotionMagnitude;
        float maxSpeed;
};

