//
// Created by samle on 02/12/2025.
//


#pragma once


#include "AI/ModuleStatus.h"
#include "AI/Modules/BaseAgentModule.h"

#include <memory>

/**
 * @brief This struct serves as an internal container for agent modules.
 * It sores the actual BaseAgentModule, the weight and the status.
 *
 * Used by @c Agent component
 */
struct ModuleData final
{
    public:
        /// Constructor takes in a unique_ptr to the module to take ownership, and its desired weight.
        ModuleData(std::unique_ptr<BaseAgentModule> agentModule, const float desiredWeight):
            module(std::move(agentModule)),
            weight(desiredWeight),
            moduleStatus(ModuleStatus::ACTIVE) /// Modules are active by default
        {
        }

        ~ModuleData() =default;

        /**
         * @brief Retrieves the @c BaseAgentModule stored by this ModuleData
         * @return A raw pointer to the module, not transferring ownership.
         */
        [[nodiscard]] BaseAgentModule* getModule() const;

        /**
         * @brief Retrieves the status of the module.
         * @return @c ModuleStatus value.
         */
        [[nodiscard]] ModuleStatus getModuleStatus() const;

        /**
         * @brief Retrieves the current weight the module this object stores has
         * @return The current weight
         */
        [[nodiscard]] float getWeight() const;

        /**
         * @brief Helper method that can be used to determine if the module is active or not
         *
         * @return true if ModuleStatus == ACTIVE, false otherwise
         */
        [[nodiscard]] bool isModuleActive() const;

        /**
         * @brief Sets the weight the module should have
         * @param weight new weight
         */
        void setWeight(float weight);

        /**
         * @brief Sets the status the module should have
         * @param status new status
         */
        void setModuleStatus(ModuleStatus status);

    private:
        /// ModuleData has ownership over the actual Module,
        /// Agent has ownership over the ModuleDatas, and operates with them instead of BaseAgentModule directly
        std::unique_ptr<BaseAgentModule> module;
        ModuleStatus moduleStatus;
        float weight;
};
