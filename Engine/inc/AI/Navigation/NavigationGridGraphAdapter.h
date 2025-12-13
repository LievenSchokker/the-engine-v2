

#pragma once


#include "Pathfinding/IPathfindingGraph.h"

class NavigationGrid;


/**
 * @brief Class to adapt the concrete implementation of the @c NavigationGrid to a @c IPathfindingGraph
 *
 * The adapter is used because a grid defines valid cells differently from what PathFinding algorithms expect.
 */
class NavigationGridGraphAdapter : public IPathfindingGraph
{
    public:
        NavigationGridGraphAdapter(const NavigationGrid& navGrid) : grid(navGrid){}
        ~NavigationGridGraphAdapter() override = default;

        std::vector<Vector2> getNeighbours(Vector2 node) const override;
        bool isValid(Vector2 node) const override;
        float getCost(Vector2 from, Vector2 to) const override;

private:
    const NavigationGrid& grid;
};


