//
// Created by samle on 08/12/2025.
//

#pragma once

class INavigationSurface;
class GameObject;
class NavigationObstacle;
class NavigationGrid;
class PathResult;
struct Vector2;
#include "AI/Navigation/IPathFinder.h"

#include <vector>
#include <memory>


class NavigationSystem
{
    public:
        NavigationSystem() = default;
        ~NavigationSystem() = default;

        std::unique_ptr<GameObject> bakeNavigationGrid(Vector2 gridSize, Vector2 cellSize, std::vector<NavigationObstacle*> navObstacles);

        PathResult computePath(const IPathFinder& pathFinder, Vector2 start, Vector2 end) const;

    private:
        std::unique_ptr<INavigationSurface> navigationSurface = nullptr;
};
