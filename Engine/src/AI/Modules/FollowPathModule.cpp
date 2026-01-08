//
// Created by samle on 15/12/2025.
//
#include "AI/Modules/FollowPathModule.h"

#include "AI/Agent.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"



Vector2 FollowPathModule::compute()
{
    if (!agent.hasPath())
        return  Vector2::zero();

    /// Note: We need to retrieve this every call because the agent's path might change.
    const PathResult& pathResult = agent.getCurrentPath();
    const auto& agentPath = pathResult.getPath();

    /// Agent has a new path:
    if (agentPath != lastKnownPath)
    {
        currentPath = agentPath;
        currentPathIndex = 0;
        lastKnownPath = currentPath;
    }

    /// Agent is at last waypoint of this path:
    if (currentPathIndex >= currentPath.size() - 1)
    {
        const Vector2& finalWaypoint = currentPath.back();

        if (Vector2::distance(agentTransform.getPosition(), finalWaypoint) < waypointRadius)
            return Vector2::zero();
    }

    /// Skip waypoints the agent has already passed
    while (currentPathIndex < currentPath.size() - 1 &&
           Vector2::distance(agentTransform.getPosition(), currentPath[currentPathIndex]) < waypointRadius)
    {
        currentPathIndex++;
    }

    /// Agent is still following along the path.
    const Vector2& currentWaypoint = currentPath[currentPathIndex];
    return (currentWaypoint - agentTransform.getPosition()).normalised();
}


void FollowPathModule::setWayPointRadius(float radius)
{
    waypointRadius = std::max(1.0f, radius);
}




