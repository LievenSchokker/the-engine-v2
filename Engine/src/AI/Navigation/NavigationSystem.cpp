//
// Created by samle on 08/12/2025.
//


#include "AI/Navigation/NavigationSystem.h"

#include "AI/Navigation/IPathFinder.h"
#include "AI/Navigation/PathResult.h"


PathResult NavigationSystem::computePath(Vector2 start, Vector2 end) const
{
    if (pathFinder == nullptr || navigationGrid == nullptr)
        return PathResult{ std::vector<Vector2>{} };

    return pathFinder->findPath(*navigationGrid, start, end);
}

NavigationGrid *NavigationSystem::bakeNavigationGrid(Vector2 gridSize, std::vector<NavigationObstacle> navObstacles)
{

}



void NavigationSystem::setPathFinder(std::unique_ptr<IPathFinder> pathFinderPtr)
{
    pathFinder = std::move(pathFinder);
}


void NavigationSystem::setNavigationGrid(const NavigationGrid &grid)
{
    navigationGrid = &grid;
}

