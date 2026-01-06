#pragma once

#include "AI/Navigation/INavigationSurface.h"

#include <memory>

class GridComponent;

/**
 * @brief Navigation surface backed by a GridComponent.
 */
class GridComponentNavigationSurface final : public INavigationSurface
{
public:
	explicit GridComponentNavigationSurface(GridComponent& gridComponent);
	~GridComponentNavigationSurface() override = default;

	const IPathfindingGraph* getPathfindingGraph() const override;
	void bakeSurface(const std::vector<BoundingBox>& obstacles) override;
	Vector2 toWorldPoint(Vector2 surfacePoint) const override;
	Vector2 toSurfacePoint(Vector2 worldPoint) const override;
	bool isPointOnSurface(Vector2 surfacePoint) const override;

private:
	GridComponent* grid = nullptr;
	std::unique_ptr<IPathfindingGraph> graphAdapter;
};
