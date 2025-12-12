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

        void setNavigationSurface(std::unique_ptr<INavigationSurface> navSurface);
        bool bakeNavigationSurface(const std::vector<BoundingBox>& obstacles) const;

        PathResult computePath(const IPathFinder& pathFinder, Vector2 start, Vector2 end) const;

    private:
        std::unique_ptr<INavigationSurface> navigationSurface = nullptr;
};
