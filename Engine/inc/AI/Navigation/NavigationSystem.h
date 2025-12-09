//
// Created by samle on 08/12/2025.
//

#pragma once
#include <vector>
class NavigationObstacle;
class NavigationGrid;
class IPathFinder;
class PathResult;
struct Vector2;

#include <memory>


class NavigationSystem
{
    public:
        NavigationSystem() = default;
        ~NavigationSystem() = default;

        PathResult computePath(Vector2 start, Vector2 end) const;
        NavigationGrid* bakeNavigationGrid(Vector2 gridSize, std::vector<NavigationObstacle> navObstacles);
        void setPathFinder(std::unique_ptr<IPathFinder> pathFinder);
        void setNavigationGrid(const NavigationGrid& navigationGrid);

    private:
        const NavigationGrid* navigationGrid = nullptr;
        std::unique_ptr<IPathFinder> pathFinder = nullptr;
};
