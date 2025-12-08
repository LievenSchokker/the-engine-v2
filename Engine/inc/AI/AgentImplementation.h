//
// Created by samle on 02/12/2025.
//

#pragma once


#include "AI/ModuleData.h"
#include "Modules/BaseAgentModule.h"


template<typename T, typename... Args>
bool Agent::addAgentModule(float desiredWeight, Args&&... args)
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::addAgentModule] T must derive from BaseAgentModule");

    /// Prevent duplicate modules.
    if (hasAgentModule<T>())
        return false;

    /// ModuleData is used to store the module (T), their weight, and the status.
    /// Agent internally stores ModuleData objects, not directly the Module T.
    /// Constructor arguments of the Module T get passed to the ModuleData constructor, which internally constructs and stores the desired Module.
    std::unique_ptr<ModuleData> moduleData = std::make_unique<ModuleData>(std::make_unique<T>(*this, std::forward<Args>(args)...), desiredWeight);

    ModuleData* added = moduleData.get();

    /// The agent has ownership over all ModuleData objects
    /// The ModuleData has ownership over the actual Module (stored in unique_ptr)
    moduleDatas.push_back(std::move(moduleData));

    /// Initialise a module right after adding it.
    added->getModule()->initialise();

    return true;
}


template<typename T>
bool Agent::removeAgentModule()
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::removeAgentModule] T must derive from BaseAgentModule");

    ModuleData* moduleData = nullptr;
    size_t index = 0;

    /// this method attempts to retrieve the ModuleData containing the module T, and it's index in the vector that stores the ModuleData objects
    /// If it succeeds, the moduleData and index variables will contain their data, and can be used to remove the desired module by removing th estored Moduledata containing it.
    if (tryGetAgentModule<T>(moduleData, index))
    {
        /// Make sure the index exists in the vector to prevent crashes
        if (index >= moduleDatas.size())
            return false;

        /// Remove the ModuleData that exists on the retrieved index
        moduleDatas.erase(std::next(moduleDatas.begin(), static_cast<ptrdiff_t>(index)));

        return true;
    }

    return false;
}


template<typename T>
bool Agent::hasAgentModule() const
{
    static_assert(std::is_base_of_v<BaseAgentModule, T>, "[Agent::hasAgentModule] T must derive from BaseAgentModule");

    ModuleData* temp = nullptr;

    /// Wrap the internally used tryGetAgentModule method to check if a module exists on the agent.
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

    /// Base case; if the module doesnt exist, return inactive.
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