//
// Created by samle on 08/12/2025.
//

#pragma once
#include "Math/Vector2.h"
struct PathResult;

class IPathFinder
{
    public:
        IPathFinder() = default;
        virtual ~IPathFinder() = default;

        virtual PathResult findPath(const NavigationGrid& grid, Vector2 start, Vector2 end) const = 0;
};
