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

    /// Return if target is not within detectRadius of the agent
    if (Vector2::distance(agentTransform.getPosition(), followTarget->getPosition()) > detectRadius)
        return Vector2::zero();

    /// Return zero if agent is within arrivingDistance of the target
    if (Vector2::distance(agentTransform.getPosition(), followTarget->getPosition()) < agent.getArrivingDistance())
        return Vector2::zero();


    /// Return a normalised vector pointing from agent towards target
    return (followTarget->getPosition() - agentTransform.getPosition()).normalised();
}


void FollowTargetModule::setFollowTarget(const Transform& target)
{
    followTarget = &target;
}


void FollowTargetModule::setDetectRadius(float radius)
{
    detectRadius = radius;
}
