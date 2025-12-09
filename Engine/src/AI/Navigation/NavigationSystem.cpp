//
// Created by samle on 08/12/2025.
//


#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationCell.h"
#include "AI/Navigation/NavigationObstacle.h"

#include "AI/Navigation/IPathFinder.h"
#include "AI/Navigation/PathResult.h"
#include "Component/GridComponent.h"

PathResult NavigationSystem::computePath(Vector2 start, Vector2 end) const
{
    if (pathFinder == nullptr || navigationGrid == nullptr)
        return PathResult{ std::vector<Vector2>{} };

    return pathFinder->findPath(navigationGrid->getCells(), start, end);
}

std::unique_ptr<GameObject> NavigationSystem::bakeNavigationGrid(Vector2 gridSize, std::vector<NavigationObstacle> navObstacles)
{
    /// Creates a GO with the grid component
    /// Lets the grid generate; a WxH grid of navCells, stored by the component
    /// Gets all obstacles and converts their bounds to cell positions
    /// Marks all cells unwalkable that are within the bounds
    /// Should store the grid component so computePath can get the correct cells

    std::unique_ptr<GameObject> gridObject = std::make_unique<GameObject>("NavigationGrid");
    navigationGrid = gridObject->addComponent<NavigationGrid>();

    navigationGrid->generateGrid(gridSize, Vector2::one());

    for (NavigationObstacle& obstacle : navObstacles)
    {
        Vector2 minCell = navigationGrid->worldToCellPosition(obstacle.getBounds().min);
        Vector2 maxCell = navigationGrid->worldToCellPosition(obstacle.getBounds().max);


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
