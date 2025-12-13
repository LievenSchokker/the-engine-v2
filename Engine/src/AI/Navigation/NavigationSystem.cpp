//
// Created by samle on 08/12/2025.
//


#include "AI/Navigation/NavigationSystem.h"

#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationObstacle.h"
#include "AI/Navigation/Pathfinding/IPathFinder.h"
#include "AI/Navigation/Pathfinding/PathResult.h"



PathResult NavigationSystem::computePath(const IPathFinder& pathFinder, Vector2 start, Vector2 end) const
{
    if (navigationSurface == nullptr)
        return PathResult::empty();

    const IPathfindingGraph* graph = navigationSurface->getPathfindingGraph();

    if (graph == nullptr)
        return PathResult::empty();

    start = navigationSurface->toSurfacePoint(start);
    end = navigationSurface->toSurfacePoint(end);

    return pathFinder.findPath(*graph, start, end);
}


void NavigationSystem::bake(const std::vector<BoundingBox>& obstacles) const
{
    if (navigationSurface == nullptr)
        return;

    navigationSurface->bakeSurface(obstacles);
}


void NavigationSystem::setNavigationSurface(std::unique_ptr<INavigationSurface> navSurface)
{
    navigationSurface = std::move(navSurface);
}

