//
// Created by samle on 04/12/2025.
//


#pragma once


#include "BaseAgentModule.h"
#include "GameObject/ObjectHandle.h"
#include "Scene/Scene.h"
class Scene;

/**
 * @brief This module attempts to follow a given target by moving the agent towards the position of the target
 */
class FollowTargetModule final : public BaseAgentModule
{
    public:
        /// Overloaded constructors so user is not required to set all values upon adding the module
        explicit FollowTargetModule(const Agent& agent) : BaseAgentModule(agent){}
        explicit FollowTargetModule(const Agent& agent, const Transform& target, float detectRadius_) : BaseAgentModule(agent), followTarget(&target), detectRadius(detectRadius_){}
        ~FollowTargetModule() override = default;

        void initialise() override;

        /**
         * @brief Computes a vector pointing towards the @c followTarget
         * @return A normalised vector towards the followTarget's position
         */
        Vector2 compute() override;

        /**
         * @brief sets the target this module should use to compute its direction towards
         * @param target transform of the target
         */
        void setFollowTarget(const Transform& target);

        /**
         * @brieg Sets the radius in which this module should attempt to compute a vector towards the target.
         * A higher radius corresponds to the agent attempting to follow its target from further away,
         * while a lower radius allows the target to come closer before the agent following him
         * @param detectRadius
         */
        void setDetectRadius(float detectRadius);

    private:
        /// The target to follow, internally used.
        /// Is retrieved every frame from the scene using the @c targetGameObjectId
		const Transform* followTarget = agentScene->getGameObject(targetGameObjectId)->getTransform();
		ObjectHandle targetGameObjectId  = ObjectHandle::null();
        Scene* agentScene = nullptr;

        /// Radius in which this module computes a vector towards the target, internally used.
        float detectRadius = 0.0f;
        bool canUpdate = false;
};
