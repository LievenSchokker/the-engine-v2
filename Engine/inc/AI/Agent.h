//
// Created by samle on 02/12/2025.
//


#pragma once


#include "Behaviour/Behaviour.h"
#include "Math/Vector2.h"

struct ModuleData;
enum class ModuleStatus;
enum class ModuleType;


class Agent final : public Behaviour
{
    public:
        Agent() : currentVelocity(Vector2::zero()), maxModuleForceMagnitude(0), maxVelocityMagnitude(0)
        {
        };

        ~Agent() override = default;

        void onAwake() override;
        void update(float deltaTime) override;

        Vector2 computeModuleForce();
        Vector2 computeDesiredVelocity();

        template<typename T, typename... Args>
        bool addAgentModule(float desiredWeight, Args &&... args);

        template<typename T>
        bool removeAgentModule();

        template<typename T>
        bool setModuleWeight(float desiredWeight) const;

        template<typename T>
        [[nodiscard]] float getModuleWeight() const;

        template<typename T>
        bool setModuleStatus(ModuleStatus status) const;

        template<typename T>
        [[nodiscard]] ModuleStatus getModuleStatus() const;

        template<typename T>
        [[nodiscard]] bool hasAgentModule() const;

        [[nodiscard]] float getMaxModuleForceMagnitude() const;
        void setMaxModuleForceMagnitude(float max);
        [[nodiscard]] float getMaxVelocityMagnitude() const;
        void setMaxVelocityMagnitude(float max);

        [[nodiscard]] size_t getModuleCount() const;

    private:
        template<typename T>
        bool tryGetAgentModule(ModuleData*& out) const;

        template<typename T>
        bool tryGetAgentModule(ModuleData*& out, size_t& index) const;

        std::vector<std::unique_ptr<ModuleData>> moduleDatas;
        Vector2 currentVelocity;
        float maxModuleForceMagnitude;
        float maxVelocityMagnitude;
};

#include "AI/AgentImplementation.h"
