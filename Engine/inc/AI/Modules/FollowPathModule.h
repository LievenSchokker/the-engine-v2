//
// Created by samle on 15/12/2025.
//

#pragma once


#include "BaseAgentModule.h"
#include "Math/Vector2.h"
#include <vector>

/**
 * @brief This agent modules attempts to steer the agent to the next waypoint along a pre-defined path.
 *
 * The module uses the agent's current path to determine the waypoints, and returns a normalised vector pointing towards the upcoming waypoint on the path.
 */
class FollowPathModule : public BaseAgentModule
{
    public:
        /**
         * @brief Default constructor sets the @c waypointRadius to 1.
         *
         * Use overloaded constructor to set another value upon construction, or use the @c setWaypointRadius() method.
         * @param agent
         */
        explicit FollowPathModule(const Agent& agent) : BaseAgentModule(agent), currentPathIndex(0), waypointRadius(2.0f) {}

        /**
         * @brief Overloaded constructor takes in a float to set as radius for agents to consider being at their next waypoint on the path.
         * @param agent
         * @param arriveRadius
         */
        explicit FollowPathModule(const Agent& agent, float arriveRadius) : BaseAgentModule(agent), currentPathIndex(0), waypointRadius(arriveRadius) {}
        ~FollowPathModule() override = default;

        /**
         * @brief Computes this modules vector by determining where the Agent is on his current path.
         *
         * The current waypoint gets tracked, increments when the agent comes within @c waypointRadius
         *
         *
         * @return a normalised vector pointing from agent towards the current waypoint on his path.
         * @return returns Vector2::zero() if agent does not have a valid path, or if the waypoint reached the end.
         */
        Vector2 compute() override;

        /**
         * @brief sets the radius of the distance the agent has to be to consider reaching a waypoint
         * @param radius the new @c waypointRadius
         */
        void setWayPointRadius(float radius);

    private:
        /// @brief Internally used to determine where the agent is on the current path.
        size_t currentPathIndex;

        /// @brief Used to determine when the agent has "reached" the next path waypoint.
        float waypointRadius;

        /// @brief Store the previous path of the agent
        std::vector<Vector2> lastKnownPath;

        /// @brief Store the current path's
        std::vector<Vector2> currentPath;
};
