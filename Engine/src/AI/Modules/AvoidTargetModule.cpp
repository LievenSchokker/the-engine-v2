//
// Created by samle on 04/12/2025.
//


#include "AI/Modules/AvoidTargetModule.h"

#include "AI/Agent.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"


void AvoidTargetModule::initialise()
{
    if (target != nullptr)
    {
        targetGameObjectId = target->getGameObject()->getSceneId();
    }

    agentScene = agent.getGameObject()->getScene();
}


Vector2 AvoidTargetModule::compute()
{
    if (target == nullptr)
        return Vector2::zero();

    GameObject* targetObject = agentScene->getGameObjectById(targetGameObjectId);

    if (targetObject == nullptr)
        return Vector2::zero();

    target = targetObject->getTransform();

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
    agentScene = agent.getGameObject()->getScene();
    targetGameObjectId = target->getGameObject()->getSceneId();
}


void AvoidTargetModule::setAvoidRadius(float radius)
{
    avoidRadius = radius;
}
