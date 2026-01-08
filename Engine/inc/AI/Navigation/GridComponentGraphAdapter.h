#pragma once

#include "AI/Navigation/Pathfinding/IPathfindingGraph.h"

class GridComponent;

/**
 * @brief Adapts a GridComponent to the IPathfindingGraph interface.
 */
class GridComponentGraphAdapter final : public IPathfindingGraph
{
public:
	explicit GridComponentGraphAdapter(const GridComponent& grid)
		: grid(grid)
	{
	}
	~GridComponentGraphAdapter() override = default;

	std::vector<Vector2> getNeighbours(Vector2 node) const override;
	bool isValid(Vector2 node) const override;
	float getCost(Vector2 from, Vector2 to) const override;

private:
	const GridComponent& grid;
};
