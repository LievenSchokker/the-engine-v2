//
// Created by samle on 02/12/2025.
//
#pragma once
#include <memory>
class BaseAgentModule;
enum class ModuleState;

struct ModuleData
{
    public:
        template <typename T, typename... Args>
        ModuleData(const float desiredWeight, const ModuleState state, Args&&... args):
            module(std::make_unique<T>(std::forward<Args>(args)...)),
            weight(desiredWeight),
            moduleState(state)
        {
            static_assert(std::is_base_of_v<BaseAgentModule, T>, "[ModuleData::constructor] T must derive from BaseAgentModule");
        }

        ~ModuleData();

        [[nodiscard]] BaseAgentModule* getModule() const;
        [[nodiscard]] ModuleState getModuleState() const;
        [[nodiscard]] float getWeight() const;

        void setWeight(const float weight);
        void setModuleStatus(ModuleState state);

    private:
        std::unique_ptr<BaseAgentModule> module;
        ModuleState moduleState;
        float weight;
};
