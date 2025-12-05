//
// Created by samle on 04/12/2025.
//


#include "AI/Modules/FollowTargetModule.h"

#include "AI/Agent.h"
#include "Component/Transform.h"


Vector2 FollowTargetModule::compute()
{
    if (followTarget == nullptr)
        return Vector2::zero();

    if (Vector2::distance(agentTransform.getPosition(), followTarget->getPosition()) < agent.getArrivingDistance())
        return Vector2::zero();

    return (followTarget->getPosition() - agentTransform.getPosition()).normalised();
}


void FollowTargetModule::setFollowTarget(const Transform& target)
{
    followTarget = &target;
}

