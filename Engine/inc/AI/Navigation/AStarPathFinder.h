//
// Created by samle on 10/12/2025.
//


#pragma once


#include "HeuristicType.h"
#include "AI/Navigation/IPathFinder.h"
#include "Math/Vector2.h"

class  NavigationGrid;
enum class HeuristicType;
struct PathResult;


/**
 * @brief Concrete IPathFinder implementation that uses the A* algorithm to compute a path from start to end position
 *
 * A HeuristicType can be provided to let the algorithm compute the cost from one node to another,
 * Default HeuristicType is set to Euclidian
 */
class AStarPathFinder final : public IPathFinder
{
    public:
        explicit AStarPathFinder();
        explicit AStarPathFinder(HeuristicType heuristic) :  heuristicType(heuristic){}
        ~AStarPathFinder() override = default;

        /**
         * @brief Finds a path on the given navigation surface from start to end coordinates (relative to the navigation surface)
         *
         * @param graph Concrete navigational graph to find a path on
         * @param start Node in the nav surface used as start by the algorithm
         * @param end The goal to find by the algorithm
         * @return A PathResult containing information about the found path
         */
        PathResult findPath(const IPathfindingGraph& graph, Vector2 start, Vector2 end) const override;

        /**
         * @brief Sets the heuristic for the algorithm to calculate the fCost from one node to one of it's neighbours
         * @param heuristic
         */
        void setHeuristicType(HeuristicType heuristic);

    private:
        /// Internally used method to calculate the heuristic from one node to another, using the selected heurisitc
        int calculateHeuristic(Vector2 from, Vector2 to) const;
        HeuristicType heuristicType;
};
