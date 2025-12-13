//
// Created by samle on 11/12/2025.
//

#pragma once


#include "Math/Vector2.h"

struct BoundingBox;
class NavigationObstacle;
class IPathfindingGraph;

#include <vector>


/**
 * @brief Abstract interface class for describing navigation surfaces used by the engine
 *
 * A navigation surface can be made and used generically, but must implement this interface to work with the engine's NavigationSystem
 *
 * Used by the NavigationSystem internally to:
 * - Create a surface with obstacles cut out
 * - Pass to the PathFinding implementations to find a path from a point of the surface to another
 */
class INavigationSurface
{
    public:
        virtual ~INavigationSurface() = 0 {}

        /**
         * @brief Wrap a specified graph abstraction used only for the pathfinding systems
         * A concrete IPathfindingGraph can access neighbours on the graph, and validate given nodes, as well as computing costs from one node to another.
         *
         * @return a const raw pointer to the IPathfindingGraph
         */
        virtual const IPathfindingGraph* getPathfindingGraph() const = 0;

        /**
         * @brief Bakes this navigation surface by baking the obstacles (represented as a generic BoundingBox) in the surface
         *
         * @param obstacles the bounding boxes to use for baking
         */
        virtual void bakeSurface(const std::vector<BoundingBox>& obstacles) = 0;

        /**
         * Converts a given Vector2 from the navigation surface to its point in the world
         * @param surfacePoint Point on the surface to convert
         * @return a Vector2 containing the world-position of the given @c surfacePoint
         */
        virtual Vector2 toWorldPoint(Vector2 surfacePoint) const = 0;

        /**
         * Converts a given Vector2 in world coordinates to a point on the navigation surface
         * @param worldPoint point in the world to convert to the surface
         * @return A Vector2 containing the coordinate on the surface
         */
        virtual Vector2 toSurfacePoint(Vector2 worldPoint) const = 0;

        /**
         * @brief Checks whether a given point converted to surface space lies on this navigation surface
         * @param surfacePoint point in the world
         * @return True if the point is on the surface, false otherwise
         */
        virtual bool isPointOnSurface(Vector2 surfacePoint) const = 0;
};
