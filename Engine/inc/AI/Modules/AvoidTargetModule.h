//
// Created by samle on 04/12/2025.
//


#pragma once

#include "AI/Modules/BaseAgentModule.h"

/**
 * @brief This module attempts to flee from a given Target, computing a vector in direct opposite towards the target
 */
class AvoidTargetModule final : public BaseAgentModule
{
    public:
        /// Overloaded constructors so user can add this module without setting all values immediately.
        explicit AvoidTargetModule(const Agent& agent): BaseAgentModule(agent){}
        explicit AvoidTargetModule(const Agent& agent, const Transform& followTarget): BaseAgentModule(agent), target(&followTarget){}
        explicit AvoidTargetModule(const Agent& agent, const Transform& followTarget, float avoidRadius): BaseAgentModule(agent), target(&followTarget), avoidRadius(avoidRadius){}

        ~AvoidTargetModule() override = default;

        /**
         * @brief Computes a vector in direct opposite of towards the target
         * @return A normalised vector, pointing away from the target
         */
        Vector2 compute() override;

        /**
         * @brief Sets the target this module should use to avoid
         * @param target Transform reference to the target
         */
        void setFollowTarget(const Transform& target);

        /**
         * @brief Sets the radius in which this module should attempt to avoid its target
         * A higher radius corresponds to an Agent attempting to flee from its target from a further distance,
         * while a shorter radius allows the target to be closer before attempting to flee
         * @param radius
         */
        void setAvoidRadius(float radius);

    private:
        /// Target to avoid.
        const Transform* target = nullptr;

        /// Radius in which this module 'activates' and attempts to avoid the target
        float avoidRadius = 0.0f;
};
