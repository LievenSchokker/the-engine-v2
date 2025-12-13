//
// Created by samle on 11/12/2025.
//

#pragma once


#include "Math/Vector2.h"
#include <vector>


/**
 * @brief Interface representing a graph usable for pathfinding algorithms.
 *
 * Abstracts the underlying navigation structure (e.g. grid, navmesh, waypoint
 * graph) and exposes only the operations required by a pathfinding algorithm.
 *
 * Nodes are represented as Vector2 values, which may correspond to grid
 * coordinates or world-space positions depending on the implementation.
 */
class IPathfindingGraph
{
    public:
        virtual ~IPathfindingGraph() = default;


        /**
        * @brief Returns all valid neighbouring nodes of a given node.
        *
        * The neighbours define the connectivity of the graph and are used by
        * pathfinding algorithms to expand search states.
        *
        * @param node The node for which neighbours are requested.
        * @return A list of neighbouring nodes reachable from the given node.
        */
        virtual std::vector<Vector2> getNeighbours(Vector2 node) const = 0;

        /**
        * @brief Returns the traversal cost between two connected nodes.
        *
        * The cost may represent distance, terrain difficulty, or any other
        * domain-specific weighting used by the pathfinding algorithm.
        *
        * @param from The starting node.
        * @param to   The destination node.
        * @return The cost of moving from @p from to @p to.
        */
        virtual float getCost(Vector2 from, Vector2 to) const = 0;

        /**
        * @brief Checks whether a node is valid within the graph.
        *
        * This can be used to determine whether a node exists, is walkable,
        * or lies within the bounds of the navigation structure.
        *
        * @param node The node to validate.
        * @return True if the node is valid and usable for pathfinding.
        */
        virtual bool isValid(Vector2 node) const = 0;
};
