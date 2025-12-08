//
// Created by samle on 08/12/2025.
//

#pragma once

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

        static PathResult findPath(Vector2 start, Vector2 end);
        void setPathFinder(std::unique_ptr<IPathFinder> pathFinder);

    private:
        NavigationGrid* navigationGrid;
        std::unique_ptr<IPathFinder> pathFinder;
};
