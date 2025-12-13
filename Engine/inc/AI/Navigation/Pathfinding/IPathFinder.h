//
// Created by samle on 08/12/2025.
//

#pragma once


#include "AI/Navigation/NavigationGrid.h"
#include "PathResult.h"
#include "Math/Vector2.h"

/**
 * @brief Interface for pathfinding algorithms.
 *
 * Defines a common contract for systems that compute a path between two
 * positions on a pathfinding graph. Implementations may use different
 * algorithms (e.g. A*, Dijkstra, BFS) while exposing the same interface.
 *
 * The pathfinder operates on an abstract graph representation, allowing
 * different grid or navigation data structures to be used interchangeably.
 */
class IPathFinder
{
    public:
        IPathFinder() = default;
        virtual ~IPathFinder() = default;

        /**
        * @brief Computes a path between two positions on a pathfinding graph.
        *
        * @param grid  The graph or navigation structure used for pathfinding.
        * @param start The starting position in graph/world coordinates.
        * @param end   The target position in graph/world coordinates.
        *
        * @return A PathResult containing the computed path and its status
        *         (e.g. success, failure, or partial path).
        */
        virtual PathResult findPath(const IPathfindingGraph& grid, Vector2 start, Vector2 end) const = 0;
};
