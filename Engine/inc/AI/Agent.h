//
// Created by samle on 02/12/2025.
//
#pragma once
#include "Behaviour/Behaviour.h"
#include <map>
#include <unordered_map>
#include "GameObject/Vector2.h"
enum class ModuleState;

namespace spelmotor_ai
{
    enum class ModuleType;
}

namespace spelmotor_ai
{
    class BaseAgentModule;
}

struct Vector2;

namespace spelmotor_ai
{
    class Agent final : public Behaviour
    {
        public:
            Agent() : currentVelocity(Vector2{0, 0}), maxMotionMagnitude(0), maxSpeed(0)
            {
            };

            ~Agent() override = default;
            void update() override;

            Vector2 computeModuleForce();
            Vector2 computeDesiredVelocity();

            bool addAgentModule(ModuleType moduleType);
            bool removeAgentModule(ModuleType moduleType);
            bool setModuleWeight(ModuleType moduleType, float desiredWeight);
            bool setModuleStatus(ModuleType module, ModuleState status);

        private:
            std::vector<std::unique_ptr<BaseAgentModule>> modules;
            std::unordered_map<ModuleType, float> moduleWeights;
            Vector2 currentVelocity;
            float maxModuleMagnitude;
            float maxVelocity;
    };
}
