//
// Created by samle on 02/12/2025.
//

#include "AI/Modules/BaseAgentModule.h"
#include "AI/ModuleData.h"


BaseAgentModule *ModuleData::getModule() const
{
    return module.get();
}


ModuleStatus ModuleData::getModuleStatus() const
{
    return moduleStatus;
}


float ModuleData::getWeight() const
{
    return weight;
}


void ModuleData::setWeight(const float desiredWeight)
{
    weight = desiredWeight;
}


void ModuleData::setModuleStatus(ModuleStatus newStatus)
{
    moduleStatus = newStatus;
}


bool ModuleData::isModuleActive() const
{
    return (moduleStatus == ModuleStatus::ACTIVE);
}



