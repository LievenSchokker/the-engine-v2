//
// Created by samle on 02/12/2025.
//


#pragma once


#include "Behaviour/Behaviour.h"
#include "Math/Vector2.h"

struct ModuleData;
enum class ModuleState;
enum class ModuleType;


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
        bool setModuleWeight(float desiredWeight) const;

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
        bool tryGetAgentModule(ModuleData*& out) const;

        template<typename T>
        bool tryGetAgentModule(ModuleData*& out, size_t& index) const;

        Transform* transform;
        std::vector<std::unique_ptr<ModuleData>> moduleDatas;
        Vector2 currentVelocity;
        float maxModuleForceMagnitude;
        float maxVelocityMagnitude;
};

#include "AI/AgentImplementation.h"
