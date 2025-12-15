//
// Created by samle on 15/12/2025.
//
#include "../../../inc/AI/Modules/FollowPathModule.h"

#include "AI/Agent.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"

Vector2 FollowPathModule::compute()
{
    if (!agent.hasPath())
        return  Vector2::zero();

    /// Note: We need to retrieve this every call because the agent's path might change.
    const PathResult& pathResult = agent.getPathResult();
    const auto& agentPathRaw = pathResult.getPath();

    /// Agent has a new path:
    if (agentPathRaw != lastRawPath)
    {
        lastRawPath = agentPathRaw;
        currentPathIndex = 0;
        worldPathPoints = convertToWorldCoordinates(agentPathRaw);
    }

    /// Agent has reached the end of his path, this module becomes "inactive".
    if (currentPathIndex >= worldPathPoints.size())
    {
        return Vector2::zero();
    }

    /// Keep track of where the agent is along the path by incrementing the index if the distance between agent and waypoint < radius.
    const Vector2& currentWaypoint = worldPathPoints[currentPathIndex];
    if (Vector2::distance(agentTransform.getPosition(), currentWaypoint) < waypointRadius)
    {
        currentPathIndex = std::min(currentPathIndex + 1, worldPathPoints.size() -1);
    }

    /// Return a normalised vector pointing from agent towards the current waypoint.
    return (currentWaypoint - agentTransform.getPosition()).normalised();
}


std::vector<Vector2> FollowPathModule::convertToWorldCoordinates(const std::vector<Vector2> &rawPathPoints)
{
    /// Need to lookup every frame to ensure the system and surface both still exist.
    auto navSystem = agent.getGameObject()->getScene().getNavigationSystem();
    if (!navSystem)
        return {};

    auto navSurface = navSystem->getNavigationSurface();
    if (!navSurface)
        return {};


    std::vector<Vector2> worldPath;
    worldPath.reserve(rawPathPoints.size());

    /// Convert everu point in the path to world coordinates using the systems's INavSurface.
    for (const auto& point : rawPathPoints)
    {
        worldPath.push_back(navSurface->toWorldPoint(point));
    }

    return worldPath;
}





