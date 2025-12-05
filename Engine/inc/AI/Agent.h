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
        Agent() : currentVelocity(Vector2::zero()), maxSpeed(1), arrivingDistance(10.0f), rotationTurnRate(90)
        {
        };

        ~Agent() override = default;

        void onAwake() override;
        void update(float deltaTime, GameWorld* gameWorld) override;

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

        [[nodiscard]] float getMaxSpeed() const;
        void setMaxSpeed(float max);

        [[nodiscard]] size_t getModuleCount() const;

        float getArrivingDistance() const;
        void setArrivingDistance(float distance);

        float getRotationTurnRate() const;
        void setRotationTurnRate(float value);

    private:
        template<typename T>
        bool tryGetAgentModule(ModuleData*& out) const;

        template<typename T>
        bool tryGetAgentModule(ModuleData*& out, size_t& index) const;

        std::vector<std::unique_ptr<ModuleData>> moduleDatas;

        Vector2 currentVelocity;

        float maxSpeed;
        float arrivingDistance;

        /// @brief The rate to turn the agent towards its current velocity with, in degrees (0 -360).
        float rotationTurnRate;
};

#include "AI/AgentImplementation.h"
