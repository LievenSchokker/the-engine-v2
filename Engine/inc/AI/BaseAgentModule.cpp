//
// Created by samle on 02/12/2025.
//


#include "BaseAgentModule.h"

#include "ModuleState.h"
#include "ModuleType.h"

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

    ModuleType BaseAgentModule::getModuleType() const
    {
        return moduleType;
    }

    void BaseAgentModule::setModuleType(ModuleType type)
    {
        moduleType = type;
    }


