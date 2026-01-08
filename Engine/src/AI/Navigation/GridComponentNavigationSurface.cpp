#include "AI/Navigation/GridComponentNavigationSurface.h"

#include "AI/Navigation/BoundingBox.h"
#include "AI/Navigation/GridComponentGraphAdapter.h"
#include "Component/GridComponent.h"

#include <algorithm>

GridComponentNavigationSurface::GridComponentNavigationSurface(
	GridComponent& gridComponent)
	: grid(&gridComponent),
	  graphAdapter(std::make_unique<GridComponentGraphAdapter>(gridComponent))
{
}

const IPathfindingGraph* GridComponentNavigationSurface::getPathfindingGraph() const
{
	return graphAdapter.get();
}

void GridComponentNavigationSurface::bakeSurface(
	const std::vector<BoundingBox>& obstacles)
{
	if (grid == nullptr || !grid->isReady())
	{
		return;
	}

	const int width = grid->getGridWidth();
	const int height = grid->getGridHeight();
	if (width <= 0 || height <= 0)
	{
		return;
	}

	for (const BoundingBox& obstacle : obstacles)
	{
		Vector2 minCell = grid->worldToCell(obstacle.min);
		Vector2 maxCell = grid->worldToCell(obstacle.max);

		int minX = static_cast<int>(std::min(minCell.x, maxCell.x));
		int maxX = static_cast<int>(std::max(minCell.x, maxCell.x));
		int minY = static_cast<int>(std::min(minCell.y, maxCell.y));
		int maxY = static_cast<int>(std::max(minCell.y, maxCell.y));

		minX = std::clamp(minX, 0, width - 1);
		maxX = std::clamp(maxX, 0, width - 1);
		minY = std::clamp(minY, 0, height - 1);
		maxY = std::clamp(maxY, 0, height - 1);

		for (int y = minY; y <= maxY; ++y)
		{
			for (int x = minX; x <= maxX; ++x)
			{
				grid->blockCell({static_cast<float>(x),
				                 static_cast<float>(y)});
			}
		}
	}
}

Vector2 GridComponentNavigationSurface::toWorldPoint(
	Vector2 surfacePoint) const
{
	if (grid == nullptr)
	{
		return Vector2::zero();
	}

	return grid->cellToWorld(surfacePoint);
}

Vector2 GridComponentNavigationSurface::toSurfacePoint(
	Vector2 worldPoint) const
{
	if (grid == nullptr)
	{
		return Vector2::zero();
	}

	return grid->worldToCell(worldPoint);
}

bool GridComponentNavigationSurface::isPointOnSurface(
	Vector2 surfacePoint) const
{
	if (grid == nullptr)
	{
		return false;
	}

	return grid->isValidCell(surfacePoint);
}
