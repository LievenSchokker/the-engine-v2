//
// Created by samle on 05/12/2025.
//
#pragma once


#include "AI/Modules/BaseAgentModule.h"
#include "Math/Vector2.h"

#include <random>


/**
 * @brief This module allows agents to move around in a realistic manner, simulating wandering around.
 */
class WanderModule final : public BaseAgentModule
{
    public:
        explicit WanderModule(const Agent &_agent) : BaseAgentModule(_agent)
        {
        }

        /// Overloaded constructor so user is not required to set all values upon adding the module.
        explicit WanderModule(const Agent &_agent, float areaDistance_, float areaRadius_, float areaJitter_)
            : BaseAgentModule(_agent), areaDistance(areaDistance_), areaRadius(areaRadius_), areaJitter(areaJitter_)
        {
        }

        ~WanderModule() override = default;

        /// Used to init a random device
        void initialise() override;

        /**
         * @brief Computes a Vector towards a point on the area in front of the agent, so he can move to it
         * @return A normalised vector towards the random selected point on the area.
         */
        Vector2 compute() override;

        /**
         * @brief Retrieves the distance the area used for computation is away from the agent's forward vector.
         * @return the value
         */
        float getAreaDistance() const;

        /**
         * @brief Retrieves the radius of the area in front of the agent's forward vector
         * @return the value
         */
        float getAreaRadius() const;

        /**
         * @brief Retrieves the jitter that is used to select a point on the area
         * @return the value
         */
        float getAreaJitter() const;

        /**
         * @brief Sets the distance from the agent at which the wander area is projected forward.
         *
         * The wander area is placed along the agent's forward direction at this distance.
         * Larger values result in smoother, more gradual direction changes,
         * while smaller values cause the agent to turn more frequently and sharply
         * (especially when combined with a large radius).
         */
        void setAreaDistance(float value);

        /**
         * @brief Sets the radius of the wander area used to select a target point.
         *
         * This radius defines how far from the center of the projected area
         * a target position can be chosen.
         * Larger values allow more variation in movement direction,
         * while smaller values constrain the agent to straighter paths.
         */
        void setAreaRadius(float value);

        /**
         * @brief Amount of random displacement applied to the wander target each update.
         *
         * Jitter adds randomness to the target position over time,
         * preventing movement from appearing static or predictable.
         * Higher values create more erratic, unpredictable motion,
         * while lower values result in smoother, more stable wandering.
         */
        void setAreaJitter(float value);

    private:
        Vector2 currentTarget = Vector2::zero();

        float getRandomBetween(float min, float max);;

        /// areaDistance controls how far ahead the agent plans its movement
        float areaDistance = 0.0f;

        /// areaRadius controls how wide the possible movement directions are
        float areaRadius = 0.0f;

        /// areaJitter controls how quickly and randomly the direction changes
        float areaJitter = 0.0f;

        /// Used to generate a random point on the area.
        std::default_random_engine randomEngine;
};
