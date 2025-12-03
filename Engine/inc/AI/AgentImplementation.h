//
// Created by samle on 02/12/2025.
//

#pragma once


#include "AI/BaseAgentModule.h"
#include "AI/ModuleData.h"
#include "AI/ModuleStatus.h"


template<typename T, typename... Args>
bool Agent::addAgentModule(float desiredWeight, Args&&... args)
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::addAgentModule] T must derive from BaseAgentModule");

    if (hasAgentModule<T>())
        return false;

    moduleDatas.emplace_back(
          std::make_unique<ModuleData>(
              std::make_unique<T>(std::forward<Args>(args)...),
              desiredWeight
          )
      );

    return true;
}


template<typename T>
bool Agent::removeAgentModule()
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::removeAgentModule] T must derive from BaseAgentModule");

    ModuleData* moduleData = nullptr;
    size_t index = 0;

    if (tryGetAgentModule<T>(moduleData, index))
    {
        moduleDatas.erase(moduleDatas.begin() + index);
        return true;
    }

    return false;
}


template<typename T>
bool Agent::hasAgentModule() const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::hasAgentModule] T must derive from BaseAgentModule");

    ModuleData* temp = nullptr;
    return tryGetAgentModule<T>(temp);
}


template<typename T>
bool Agent::setModuleWeight(float desiredWeight) const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::setModuleWeight] T must derive from BaseAgentModule");

    ModuleData* moduleData = nullptr;

    if (tryGetAgentModule<T>(moduleData))
    {
        moduleData->setWeight(desiredWeight);
        return true;
    }

    return false;

}


template<typename T>
float Agent::getModuleWeight() const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::getModuleWeight] T must derive from BaseAgentModule");

    ModuleData* moduleData = nullptr;

    if (tryGetAgentModule<T>(moduleData))
    {
        return moduleData->getWeight();
    }

    return 0;
}


template<typename T>
bool Agent::setModuleStatus(ModuleStatus status) const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::setModuleStatus] T must derive from BaseAgentModule");

    ModuleData* moduleData = nullptr;

    if (tryGetAgentModule<T>(moduleData))
    {
        moduleData->setModuleStatus(status);
        return true;
    }

    return false;
}


template<typename T> ModuleStatus Agent::getModuleStatus() const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::getModuleStatus] T must derive from BaseAgentModule");

    ModuleData* moduleData = nullptr;

    if (tryGetAgentModule<T>(moduleData))
    {
        return moduleData->getModuleStatus();
    }

    return ModuleStatus::INACTIVE;
}


template<typename T>
bool Agent::tryGetAgentModule(ModuleData*& out) const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::tryGetAgentModule] T must derive from BaseAgentModule");

    for (const std::unique_ptr<ModuleData>& moduleData : moduleDatas)
    {
        if (dynamic_cast<T*>(moduleData->getModule()))
        {
            out = moduleData.get();
            return true;
        }
    }

    out = nullptr;
    return false;
}


template<typename T>
bool Agent::tryGetAgentModule(ModuleData*& out, size_t& index) const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::tryGetAgentModule] T must derive from BaseAgentModule");

    for (size_t i = 0; i < moduleDatas.size(); ++i)
    {
        if (dynamic_cast<T*>(moduleDatas[i]->getModule()))
        {
            out = moduleDatas[i].get();
            index = i;
            return true;
        }
    }

    out = nullptr;
    index = -1;
    return false;
}