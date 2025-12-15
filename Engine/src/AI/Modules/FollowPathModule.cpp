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

    const PathResult& agentPath = agent.getPathResult();

    if (&agentPath != currentPath || agentPath.getPathSize() != currentPath->getPathSize())
    {
        currentPathIndex = 0;
        currentPath = &agentPath;
    }

    if (currentPathIndex >= currentPath->getPathSize())
        return Vector2::zero();

    std::vector<Vector2> pathPointsWorld = convertToWorldCoordinates(currentPath->getPath());

    if (Vector2::distance(agentTransform.getPosition(), pathPointsWorld[currentPathIndex]) < waypointRadius)
    {
        currentPathIndex = std::min(currentPathIndex +1, currentPath->getPathSize() -1);
    }

    return (pathPointsWorld[currentPathIndex] - agentTransform.getPosition()).normalised();
}


std::vector<Vector2> FollowPathModule::convertToWorldCoordinates(const std::vector<Vector2> &rawPathPoints)
{
    auto navSystem = agent.getGameObject()->getScene().getNavigationSystem();
    if (!navSystem) return {};

    auto navSurface = navSystem->getNavigationSurface();
    if (!navSurface) return {};

    std::vector<Vector2> worldPath;
    worldPath.reserve(rawPathPoints.size());

    for (const auto& point : rawPathPoints)
    {
        worldPath.push_back(navSurface->toWorldPoint(point));
    }

    return worldPath;
}





