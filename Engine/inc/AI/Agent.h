//
// Created by samle on 02/12/2025.
//
#pragma once
#include "Behaviour/Behaviour.h"
#include <map>
#include <unordered_map>
#include "GameObject/Vector2.h"
enum class ModuleState;
enum class ModuleType;
class BaseAgentModule;
struct Vector2;

class Agent final : public Behaviour
{
    public:
        Agent() : currentVelocity(Vector2{0, 0}), maxModuleMagnitude(0), maxVelocity(0)
        {
        };

        ~Agent() override = default;

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

    private:
        template<typename T>
        [[nodiscard]] BaseAgentModule* getAgentModule() const;

        std::vector<std::unique_ptr<BaseAgentModule> > modules;
        std::unordered_map<ModuleType, float> moduleWeights;
        Vector2 currentVelocity;
        float maxModuleMagnitude;
        float maxVelocity;
};
