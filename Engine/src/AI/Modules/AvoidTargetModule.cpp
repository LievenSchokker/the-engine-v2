//
// Created by samle on 04/12/2025.
//


#include "AI/Modules/AvoidTargetModule.h"
#include "Component/Transform.h"


Vector2 AvoidTargetModule::compute()
{
    if (followTarget == nullptr)
        return Vector2::zero();

    return -(followTarget->getPosition() - agentTransform.getPosition()).normalised();
}

void AvoidTargetModule::setFollowTarget(const Transform &target)
{
    followTarget = &target;
}

