//
// Created by samle on 02/12/2025.
//

#pragma once



#include "AI/Agent.h"
#include "AI/BaseAgentModule.h"
#include "AI/ModuleData.h"

template<typename T, typename... Args>
bool Agent::addAgentModule(float desiredWeight, Args&&... args)
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::addAgentModule] T must derive from BaseAgentModule");

    if (hasAgentModule<T>())
        return false;

    auto module = modules.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    moduleWeights[module->getModuleType()] = desiredWeight;

    return true;
}

template<typename T>
bool Agent::removeAgentModule()
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::removeAgentModule] T must derive from BaseAgentModule");

    if (!hasAgentModule<T>())
        return false;

    auto it = std::find_if(modules.begin(),
                   modules.end(),
                   [](const std::unique_ptr<BaseAgentModule> &module)
                   {
                       return dynamic_cast<T*>(module.get()) != nullptr;
                   });

    if (it != modules.end())
    {
        modules.erase(it);
        moduleWeights.erase(it->get()->getModuleType());
        return true;
    }

    return false;
}


template<typename T>
bool Agent::hasAgentModule() const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::hasAgentModule] T must derive from BaseAgentModule");

    const BaseAgentModule* module = getAgentModule<T>();

    return (module != nullptr);
}

template<typename T>
bool Agent::setModuleWeight(float desiredWeight)
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::setModuleWeight] T must derive from BaseAgentModule");

    if (!hasAgentModule<T>())
        return false;

    BaseAgentModule* module = getAgentModule<T>();

    if (module == nullptr)
        return false;


    moduleWeights[module->getModuleType()] = desiredWeight;
    return true;
}

template<typename T>
bool Agent::setModuleStatus(ModuleState status) const
{
    BaseAgentModule* module = getAgentModule<T>();

    if (module == nullptr)
        return false;

    module->setModuleState(status);
    return true;
}


template<typename T>
BaseAgentModule* Agent::getAgentModule() const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::hasAgentModule] T must derive from BaseAgentModule");

    auto it = std::find_if(modules.begin(),
                   modules.end(),
                   [](const std::unique_ptr<BaseAgentModule> &module)
                   {
                       return dynamic_cast<T*>(module.get()) != nullptr;
                   }
       );

        if (it != modules.end())
        {
            return it->get();
        }

    return nullptr;
}