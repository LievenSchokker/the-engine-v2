//
// Created by samle on 08/12/2025.
//

#pragma once


#include "Math/Vector2.h"
#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/PathResult.h"


class IPathFinder
{
    public:
        IPathFinder() = default;
        virtual ~IPathFinder() = default;

        virtual PathResult findPath(const IGraph& grid, Vector2 start, Vector2 end) const = 0;
};
