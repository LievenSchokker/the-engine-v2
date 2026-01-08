#include "AI/Navigation/GridComponentGraphAdapter.h"

#include "Component/GridComponent.h"

#include <cmath>

float GridComponentGraphAdapter::getCost(Vector2 from, Vector2 to) const
{
	Vector2 delta = to - from;
	const bool isDiagonal =
		(std::abs(delta.x) > 0.0f && std::abs(delta.y) > 0.0f);

	const float movementCost =
		isDiagonal ? static_cast<float>(std::sqrt(2.0)) : 1.0f;
	const float cellCost = static_cast<float>(grid.getCellWeight(to));

	return movementCost + cellCost;
}

std::vector<Vector2> GridComponentGraphAdapter::getNeighbours(
	Vector2 node) const
{
	return grid.getNeighbors(node);
}

bool GridComponentGraphAdapter::isValid(Vector2 node) const
{
	return grid.isWalkable(node);
}
