//
// Created by samle on 15/12/2025.
//
#include "../../../inc/AI/Modules/FollowPathModule.h"

#include "AI/Agent.h"
#include "Component/Transform.h"


Vector2 FollowPathModule::compute()
{
    if (!agent.hasPath())
        return  Vector2::zero();

    const PathResult& agentPath = agent.getPathResult();

    if (&agentPath != currentPath || agentPath.getPathSize() != currentPath->getPathSize())
    {
        currentPathIndex = 0;
        currentPath = &agentPath;
    }

    if (currentPathIndex >= currentPath->getPathSize())
        return Vector2::zero();

    const std::vector<Vector2>& pathPoints = currentPath->getPath();

    if (Vector2::distance(agentTransform.getPosition(), pathPoints[currentPathIndex]) < waypointRadius)
    {
        currentPathIndex = std::min(currentPathIndex +1, currentPath->getPathSize() -1);
    }

    return (pathPoints[currentPathIndex] - agentTransform.getPosition()).normalised();
}


