//
// Created by samle on 15/12/2025.
//

#pragma once
#include <filesystem>
#include <vector>
#include <__msvc_filebuf.hpp>

#include "BaseAgentModule.h"
#include "AI/Navigation/Pathfinding/PathResult.h"

/**
 * @brief This agent modules attempts to steer the agent to the next waypoint along a pre-defined path.
 *
 * The module uses the agent's current path to determine the waypoints, and returns a normalised vector pointing towards the upcoming waypoint on the path.
 */
class FollowPathModule : public BaseAgentModule
{
    public:
        explicit FollowPathModule(const Agent& agent) : BaseAgentModule(agent), currentPathIndex(0), waypointRadius(3.0f), lastRawPath({}) {}
        ~FollowPathModule() override = default;

        Vector2 compute() override;
        std::vector<Vector2> convertToWorldCoordinates(const std::vector<Vector2>& rawPathPoints);

    private:
        size_t currentPathIndex;
        float waypointRadius;
        const PathResult* currentPath;
        std::vector<Vector2> lastRawPath;
        std::vector<Vector2> worldPathPoints;
};
