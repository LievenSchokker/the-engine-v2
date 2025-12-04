//
// Created by samle on 02/12/2025.
//

#pragma once

#include "AI/ModuleStatus.h"
class BaseAgentModule;

#include <memory>


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

        [[nodiscard]] bool isModuleActive() const;

        void setWeight(float weight);
        void setModuleStatus(ModuleStatus state);

    private:
        std::unique_ptr<BaseAgentModule> module;
        ModuleStatus moduleStatus;
        float weight;

};
