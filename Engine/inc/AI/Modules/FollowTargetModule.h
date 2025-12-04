//
// Created by samle on 04/12/2025.
//


#include "BaseAgentModule.h"

class FollowTargetModule final : public BaseAgentModule
{
    public:
        explicit FollowTargetModule(const Agent& agent) : BaseAgentModule(agent){}
        explicit FollowTargetModule(const Agent& agent, const Transform& target) : BaseAgentModule(agent){}
        ~FollowTargetModule() override = default;

        Vector2 compute() override;
        void setFollowTarget(const Transform& target);


    private:
        const Transform* followTarget = nullptr;
};
