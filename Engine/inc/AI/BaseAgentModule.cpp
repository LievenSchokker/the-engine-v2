//
// Created by samle on 02/12/2025.
//


#include "BaseAgentModule.h"

#include "ModuleData.h"
#include "ModuleState.h"

namespace spelmotor_ai
{
    bool BaseAgentModule::isActive() const
    {
        return (moduleState == ModuleState::ACTIVE);
    }

    ModuleState BaseAgentModule::getModuleState() const
    {
        return moduleState;
    }

    void BaseAgentModule::setModuleState(const ModuleState state)
    {
        moduleState = state;
    }
}
