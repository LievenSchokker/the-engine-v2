//
// Created by samle on 02/12/2025.
//


#include "AI/ModuleData.h"


BaseAgentModule *ModuleData::getModule() const
{
    return module.get();
}

ModuleState ModuleData::getModuleState() const
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

void ModuleData::setModuleStatus(ModuleState newStatus)
{
    moduleStatus = newStatus;
}




