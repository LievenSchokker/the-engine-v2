//
// Created by samle on 02/12/2025.
//
#pragma once
#include "ModuleState.h"
enum class ModuleType;
struct Vector2;

class BaseAgentModule
{
    public:
        BaseAgentModule() : moduleState(ModuleState::ACTIVE)
        {
        };

        virtual ~BaseAgentModule() = default;

        /**
         * Computes the Vector needed to move the agent based on this module's logic.
         * @return the computed movement vector for this module.
         */
        virtual Vector2 compute() = 0;

        /**
         * @return the current ModuleState of this module
         */
        ModuleState getModuleState() const;

        ModuleType getModuleType() const;

        /**
         * Sets the current ModuleState of this module
         * @param state the new state
         */
        void setModuleState(ModuleState state);

        void setModuleType(ModuleType type);

        /**
         * Uses the module state to determine if this module is active
         * @return true if ModuleState == active, false otherwise.
         */
        bool isActive() const;

    protected:
        ModuleState moduleState;
        ModuleType moduleType;
};
