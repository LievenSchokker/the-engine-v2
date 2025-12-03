//
// Created by samle on 02/12/2025.
//
#pragma once
#include <memory>

#include "AI/ModuleStatus.h"
#include "AI/BaseAgentModule.h"

struct ModuleData final
{
    public:
        ModuleData(std::unique_ptr<BaseAgentModule> agentModule, const float desiredWeight):
            module(std::move(agentModule)),
            weight(desiredWeight),
            moduleStatus(ModuleStatus::ACTIVE)
        {
        }

        ~ModuleData() =default;

        [[nodiscard]] BaseAgentModule* getModule() const;
        [[nodiscard]] ModuleStatus getModuleStatus() const;
        [[nodiscard]] float getWeight() const;

        void setWeight(float weight);
        void setModuleStatus(ModuleStatus state);

    private:
        std::unique_ptr<BaseAgentModule> module;
        ModuleStatus moduleStatus;
        float weight;
};
