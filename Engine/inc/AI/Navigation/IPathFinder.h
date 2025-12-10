//
// Created by samle on 08/12/2025.
//

#pragma once
#include <vector>

#include "Math/Vector2.h"
struct NavigationCell;
struct PathResult;

class IPathFinder
{
    public:
        IPathFinder() = default;
        virtual ~IPathFinder() = default;

        virtual PathResult findPath(std::vector<NavigationCell> navigatableCells, Vector2 start, Vector2 end) const = 0;
};
