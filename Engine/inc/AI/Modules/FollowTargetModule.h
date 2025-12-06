//
// Created by samle on 04/12/2025.
//


#include "BaseAgentModule.h"

class FollowTargetModule final : public BaseAgentModule
{
    public:
        explicit FollowTargetModule(const Agent& agent) : BaseAgentModule(agent){}
        explicit FollowTargetModule(const Agent& agent, const Transform& target, float detectRadius_) : BaseAgentModule(agent), followTarget(&target), detectRadius(detectRadius_){}
        ~FollowTargetModule() override = default;

        Vector2 compute() override;
        void setFollowTarget(const Transform& target);
        void setDetectRadius(float detectRadius);

    private:
        const Transform* followTarget = nullptr;
        float detectRadius = 0.0f;
};
