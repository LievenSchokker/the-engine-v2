

#pragma once


#include "AI/Navigation/IPathfindingGraph.h"

class NavigationGrid;


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


