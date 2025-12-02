//
// Created by samle on 02/12/2025.
//
#pragma once
#include "ModuleState.h"

namespace spelmotor_ai
{
    enum class ModuleState;
}

struct Vector2;

namespace spelmotor_ai
{
    class BaseAgentModule
    {
        public:
            BaseAgentModule();
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

            /**
             * Sets the current ModuleState of this module
             * @param state the new state
             */
            void setModuleState(ModuleState state);

            /**
             * Uses the module state to determine if this module is active
             * @return true if ModuleState == active, false otherwise.
             */
            bool isActive() const;

        protected:
            ModuleState moduleState;
    };
}
