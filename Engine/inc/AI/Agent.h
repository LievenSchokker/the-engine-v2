//
// Created by samle on 02/12/2025.
//
#pragma once
#include "Behaviour/Behaviour.h"
#include <map>
#include <unordered_map>
#include "Math/Vector2.h"
struct ModuleData;
enum class ModuleState;
enum class ModuleType;
#include "AI/BaseAgentModule.h"

class Agent final : public Behaviour
{
    public:
        Agent() : transform(nullptr),currentVelocity(Vector2{0, 0}), maxModuleForceMagnitude(0), maxVelocityMagnitude(0)
        {
        };

        ~Agent() override = default;

        void onAwake() override;
        void update() override;

        Vector2 computeModuleForce();
        Vector2 computeDesiredVelocity();

        template<typename T, typename... Args>
        bool addAgentModule(float desiredWeight, Args &&... args);

        template<typename T>
        bool removeAgentModule();

        template<typename T>
        bool setModuleWeight(float desiredWeight);

        template<typename T>
        [[nodiscard]] bool setModuleStatus(ModuleState status) const;

        template<typename t>
        [[nodiscard]] bool hasAgentModule() const;

        float getMaxModuleForceMagnitude() const;
        void setMaxModuleForceMagnitude(float max);
        float getMaxVelocityMagnitude() const;
        void setMaxVelocityMagnitude(float max);

    private:
        template<typename T>
        [[nodiscard]] BaseAgentModule* getAgentModule() const;

        Transform* transform;
        std::vector<std::unique_ptr<BaseAgentModule> > modules;
        std::vector<std::unique_ptr<ModuleData>> moduleDatas;
        std::unordered_map<ModuleType, float> moduleWeights;
        Vector2 currentVelocity;
        float maxModuleForceMagnitude;
        float maxVelocityMagnitude;
};

#include "AI/AgentImplementation.h"
