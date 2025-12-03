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
        ModuleData(const float desiredWeight, const ModuleState state, Args&&... args)
            : module(std::make_unique<T>(std::forward<Args>(args)...)), weight(desiredWeight), moduleState(state) {}
        ~ModuleData();

        /// TEMP PUBLIC:
    public:
        std::unique_ptr<BaseAgentModule> module;
        ModuleState moduleState;
        float weight;
};
