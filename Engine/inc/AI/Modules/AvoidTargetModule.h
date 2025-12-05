//
// Created by samle on 04/12/2025.
//


#pragma once

#include "AI/Modules/BaseAgentModule.h"


class AvoidTargetModule final : public BaseAgentModule
{
    public:
        explicit AvoidTargetModule(const Agent& agent): BaseAgentModule(agent){}
        explicit AvoidTargetModule(const Agent& agent, const Transform& followTarget): BaseAgentModule(agent), followTarget(&followTarget){}
        explicit AvoidTargetModule(const Agent& agent, const Transform& followTarget, float avoidRadius): BaseAgentModule(agent), followTarget(&followTarget), avoidRadius(avoidRadius){}

        ~AvoidTargetModule() override = default;

        Vector2 compute() override;
        void setFollowTarget(const Transform& target);
        void setAvoidRadius(float radius);

    private:
        const Transform* followTarget = nullptr;
        float avoidRadius = 0.0f;
};
