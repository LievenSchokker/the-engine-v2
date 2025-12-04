//
// Created by samle on 04/12/2025.
//


#pragma once

#include "AI/Modules/BaseAgentModule.h"


class AvoidTargetModule final : public BaseAgentModule
{
    public:
        explicit AvoidTargetModule(const Agent& agent): BaseAgentModule(agent){}
        explicit AvoidTargetModule(const Agent& agent, const Transform& followTarget): BaseAgentModule(agent){}

        ~AvoidTargetModule() override = default;

        Vector2 compute() override;
        void setFollowTarget(const Transform& target);

    private:
        const Transform* followTarget = nullptr;
};
