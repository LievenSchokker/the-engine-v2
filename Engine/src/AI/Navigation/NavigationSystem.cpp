//
// Created by samle on 08/12/2025.
//


#include "AI/Navigation/NavigationSystem.h"

#include "AI/Navigation/AStarPathFinder.h"
#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationObstacle.h"
#include "AI/Navigation/IPathFinder.h"
#include "AI/Navigation/PathResult.h"
#include "AI/Navigation/IPathFinder.h"
#include "Component/GridComponent.h"



PathResult NavigationSystem::computePath(const IPathFinder& pathFinder, Vector2 start, Vector2 end) const
{
    if (navigationSurface == nullptr)
        return PathResult { std::vector<Vector2>{} };

    const IPathfindingGraph* graph = navigationSurface->getPathfindingGraph();

    if (graph == nullptr)
        return PathResult { std::vector<Vector2>{} };

    start = navigationSurface->toSurfacePoint(start);
    end = navigationSurface->toSurfacePoint(end);

    return pathFinder.findPath(*graph, start, end);
}

std::unique_ptr<GameObject> NavigationSystem::bakeNavigationGrid(Vector2 gridSize, Vector2 cellSize, std::vector<NavigationObstacle*> navObstacles)
{
    std::unique_ptr<GameObject> gridObject = std::make_unique<GameObject>("NavigationGrid");
    navigationGrid = gridObject->addComponent<NavigationGrid>();

    navigationGrid->generateGrid(gridSize, cellSize);

    for (NavigationObstacle* obstacle : navObstacles)
    {
        Vector2 minCell = navigationGrid->worldToCellPosition(obstacle->getBounds().min);
        Vector2 maxCell = navigationGrid->worldToCellPosition(obstacle->getBounds().max);


        for (int y = minCell.y; y <= maxCell.y; ++y)
        {
            for (int x = minCell.x; x <= maxCell.x; ++x)
            {
                if (navigationGrid->isValidCell({static_cast<float>(x), static_cast<float>(y)}))
                {
                    navigationGrid->setWalkable({static_cast<float>(x), static_cast<float>(y)}, false);
                }
            }
        }
    }

    return gridObject;
}


void NavigationSystem::setPathFinder(std::unique_ptr<IPathFinder> pathFinderPtr)
{
    pathFinder = std::move(pathFinderPtr);
}
