//
// Created by samle on 04/12/2025.
//


#include "AI/Modules/AvoidTargetModule.h"
#include "Component/Transform.h"


Vector2 AvoidTargetModule::compute()
{
    /// Return if target is MIA
    if (target == nullptr)
        return Vector2::zero();

    /// Return if avoidRadius is not set
    if (avoidRadius <= 0)
        return Vector2::zero();

    /// Return if the target is outside the avoiding radius
    if (Vector2::distance(agentTransform.getPosition(), target->getPosition()) > avoidRadius)
        return Vector2::zero();

    /// Return a normalised vector, pointing directly away from the target
    return -(target->getPosition() - agentTransform.getPosition()).normalised();
}

void AvoidTargetModule::setFollowTarget(const Transform& newTarget)
{
    target = &newTarget;
}


void AvoidTargetModule::setAvoidRadius(float radius)
{
    avoidRadius = radius;
}
