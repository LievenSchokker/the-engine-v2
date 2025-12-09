//
// Created by samle on 08/12/2025.
//

#pragma once

class GameObject;
class NavigationObstacle;
class NavigationGrid;
class IPathFinder;
class PathResult;
struct Vector2;

#include <vector>
#include <memory>


class NavigationSystem
{
    public:
        NavigationSystem() = default;
        ~NavigationSystem() = default;

        std::unique_ptr<GameObject> bakeNavigationGrid(Vector2 gridSize, std::vector<NavigationObstacle> navObstacles);
        void setPathFinder(std::unique_ptr<IPathFinder> pathFinder);

        PathResult computePath(Vector2 start, Vector2 end) const;

    private:
        NavigationGrid* navigationGrid = nullptr;
        std::unique_ptr<IPathFinder> pathFinder = nullptr;
};
