//
// Created by samle on 11/12/2025.
//

#pragma once

#include "Math/Vector2.h"

#include <memory>
#include <vector>

struct BoundingBox;
class NavigationObstacle;
class IPathfindingGraph;


class INavigationSurface
{
    public:
        virtual ~INavigationSurface() = 0;
        virtual const IPathfindingGraph* getPathfindingGraph() const = 0;
        virtual void createSurface(Vector2 size, const std::vector<BoundingBox>& obstacles);
        virtual Vector2 toWorldPoint(Vector2 surfacePoint) = 0;
        virtual Vector2 toSurfacePoint(Vector2 worldPoint) = 0;
};
