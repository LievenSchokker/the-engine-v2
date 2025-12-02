//
// Created by samle on 02/12/2025.
//

#pragma once

#include <memory>
class BaseAgentModule;
enum class ModuleState;

namespace spelmotor_ai
{
    struct  ModuleData
    {
        public:
            ModuleData();
            ~ModuleData();

        private:
            std::unique_ptr<BaseAgentModule> module;
            ModuleState moduleState;
            float weight;
    };
}