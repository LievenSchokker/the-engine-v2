#include "Component/GridComponent.h"

#include "Component/TilemapComponent.h"
#include "Component/Transform.h"

#include <algorithm>
#include <cmath>

GridComponent::GridComponent()
{
}

void GridComponent::setTilemapComponent(TilemapComponent* tilemap)
{
	tilemapComponent = tilemap;
}

bool GridComponent::isWalkable(Vector2 cell) const
{
	if ( tilemapComponent == nullptr || !tilemapComponent->isReady() ) {
		return false;
	}

	if ( !isValidCell(cell) ) {
		return false;
	}

	if ( isCellBlocked(cell) ) {
		return false;
	}

	// Tile ID 0 means empty/air = walkable
	int tileId = tilemapComponent->getTileAt(cell);
	return isTileWalkable(tileId);
}

bool GridComponent::isWalkableAtWorld(Vector2 worldPos) const
{
	if ( tilemapComponent == nullptr || !tilemapComponent->isReady() ) {
		return false;
	}

	Vector2 cell = tilemapComponent->worldToCell(worldPos);
	return isWalkable(cell);
}

void GridComponent::setWalkableTileIds(const std::vector<int>& tileIds)
{
	walkableTileIds.clear();
	for ( int id : tileIds ) {
		walkableTileIds.insert(id);
	}
}

void GridComponent::addWalkableTileId(int tileId)
{
	walkableTileIds.insert(tileId);
}

void GridComponent::clearWalkableTileIds()
{
	walkableTileIds.clear();
}

void GridComponent::blockCell(Vector2 cell)
{
	blockedCells.insert(cellKey(cell));
}

void GridComponent::unblockCell(Vector2 cell)
{
	blockedCells.erase(cellKey(cell));
}

bool GridComponent::isCellBlocked(Vector2 cell) const
{
	return blockedCells.contains(cellKey(cell));
}

void GridComponent::clearBlockedCells()
{
	blockedCells.clear();
}

void GridComponent::setTileWeight(int tileId, double weight)
{
	tileWeights[tileId] = weight;
}

double GridComponent::getTileWeight(int tileId) const
{
	auto it = tileWeights.find(tileId);
	if ( it != tileWeights.end() ) {
		return it->second;
	}
	return 1.0;	 // Default weight
}

double GridComponent::getCellWeight(Vector2 cell) const
{
	if ( tilemapComponent == nullptr || !tilemapComponent->isReady() ) {
		return 1.0;
	}

	int tileId = tilemapComponent->getTileAt(cell);
	return getTileWeight(tileId);
}

std::vector<Vector2> GridComponent::getWalkableNeighbors(Vector2 cell) const
{
	std::vector<Vector2> neighbors = getNeighbors(cell, false);

	// Filter to only walkable neighbors
	std::vector<Vector2> walkableNeighbors;
	for ( const auto& neighbor : neighbors ) {
		if ( isWalkable(neighbor) ) {
			walkableNeighbors.push_back(neighbor);
		}
	}

	return walkableNeighbors;
}

std::vector<Vector2> GridComponent::getNeighbors(Vector2 cell,
												 bool includeDiagonals) const
{
	std::vector<Vector2> neighbors;

	// 4-directional neighbors (cardinal directions)
	neighbors.push_back({cell.x, cell.y - 1});	// Up
	neighbors.push_back({cell.x, cell.y + 1});	// Down
	neighbors.push_back({cell.x - 1, cell.y});	// Left
	neighbors.push_back({cell.x + 1, cell.y});	// Right

	// 8-directional neighbors (including diagonals)
	if ( includeDiagonals ) {
		neighbors.push_back({cell.x - 1, cell.y - 1});	// Up-Left
		neighbors.push_back({cell.x + 1, cell.y - 1});	// Up-Right
		neighbors.push_back({cell.x - 1, cell.y + 1});	// Down-Left
		neighbors.push_back({cell.x + 1, cell.y + 1});	// Down-Right
	}

	// Filter out invalid cells
	std::vector<Vector2> validNeighbors;
	for ( const auto& neighbor : neighbors ) {
		if ( isValidCell(neighbor) ) {
			validNeighbors.push_back(neighbor);
		}
	}

	return validNeighbors;
}

double GridComponent::manhattanDistance(Vector2 from, Vector2 to)
{
	return std::abs(to.x - from.x) + std::abs(to.y - from.y);
}

double GridComponent::euclideanDistance(Vector2 from, Vector2 to)
{
	double dx = to.x - from.x;
	double dy = to.y - from.y;
	return std::sqrt(dx * dx + dy * dy);
}

bool GridComponent::isValidCell(Vector2 cell) const
{
	if ( tilemapComponent == nullptr || !tilemapComponent->isReady() ) {
		return false;
	}

	int x = static_cast<int>(cell.x);
	int y = static_cast<int>(cell.y);

	return x >= 0 && x < tilemapComponent->getGridWidth() && y >= 0 &&
		   y < tilemapComponent->getGridHeight();
}

bool GridComponent::isTileWalkable(int tileId) const
{
	return walkableTileIds.contains(tileId);
}

std::pair<int, int> GridComponent::cellKey(Vector2 cell)
{
	return {static_cast<int>(std::floor(cell.x)),
			static_cast<int>(std::floor(cell.y))};
}

int GridComponent::getGridWidth() const
{
	if ( tilemapComponent == nullptr ) {
		return 0;
	}
	return tilemapComponent->getGridWidth();
}

int GridComponent::getGridHeight() const
{
	if ( tilemapComponent == nullptr ) {
		return 0;
	}
	return tilemapComponent->getGridHeight();
}

void GridComponent::setDebugRenderEnabled(bool enabled)
{
	debugRenderEnabled = enabled;
}

bool GridComponent::isDebugRenderEnabled() const
{
	return debugRenderEnabled;
}

bool GridComponent::isDebugShowDiagonalLinks() const
{
	return debugShowDiagonalLinks;
}

void GridComponent::setDebugWalkableColor(const Color& color)
{
	debugWalkableDotColor = color;
}

void GridComponent::setDebugBlockedColor(const Color& color)
{
	debugBlockedDotColor = color;
}

void GridComponent::setDebugGridLineColor(const Color& color)
{
	debugGridLineColor = color;
}

void GridComponent::setDebugShowDiagonalLinks(bool enabled)
{
	debugShowDiagonalLinks = enabled;
}

std::vector<ShapeRenderCommand> GridComponent::buildDebugRenderCommands() const
{
	std::vector<ShapeRenderCommand> commands;

	if ( !debugRenderEnabled || tilemapComponent == nullptr ||
		 !tilemapComponent->isReady() ) {
		return commands;
	}

	const Transform* transform = getTransform();
	if ( transform == nullptr ) {
		// Try to get transform from tilemap's GameObject
		if ( tilemapComponent->getGameObject() != nullptr ) {
			transform = tilemapComponent->getGameObject()->getTransform();
		}
		if ( transform == nullptr ) {
			return commands;
		}
	}

	const Vector2 origin = transform->getPosition();
	const Vector2 tileSize = tilemapComponent->getTileSize();
	const int width = tilemapComponent->getGridWidth();
	const int height = tilemapComponent->getGridHeight();

	// Calculate dot size (small circle at center of walkable tiles)
	const double dotRadius =
		std::min(tileSize.x, tileSize.y) * 0.1;	 // 10% of tile size

	// Calculate line thickness (for connections between walkable tiles)
	const double lineThickness =
		std::min(tileSize.x, tileSize.y) * 0.05;  // 5% of tile size

	// First pass: Collect walkable tile centers (don't draw yet)
	std::vector<std::pair<Vector2, Vector2>>
		walkableCenters;  // (cell, worldCenter)

	for ( int y = 0; y < height; ++y ) {
		for ( int x = 0; x < width; ++x ) {
			Vector2 cell{static_cast<double>(x), static_cast<double>(y)};

			// Only render on walkable tiles (tile ID == 0)
			if ( !isWalkable(cell) ) {
				continue;
			}

			// Calculate tile center position
			Vector2 tileCenter{};
			tileCenter.x = origin.x + (x * tileSize.x) + (tileSize.x / 2.0);
			tileCenter.y = origin.y + (y * tileSize.y) + (tileSize.y / 2.0);

			// Store center for line and dot drawing
			walkableCenters.push_back({cell, tileCenter});
		}
	}

	// Second pass: Draw lines FIRST (so they appear below dots)
	// Lines connecting walkable tiles to their walkable neighbors
	// Includes diagonals for visualization purposes
	for ( const auto& [cell, center] : walkableCenters ) {
		// Use diagonals for debug visualization when requested while
		// pathfinding logic elsewhere can still choose 4-directional neighbors
		// if needed
		std::vector<Vector2> neighbors =
			getNeighbors(cell, debugShowDiagonalLinks);

		for ( const auto& neighborCell : neighbors ) {
			// Only draw line if neighbor is also walkable
			if ( !isWalkable(neighborCell) ) {
				continue;
			}

			// Find the world center of the neighbor
			Vector2 neighborCenter{};
			neighborCenter.x =
				origin.x + (neighborCell.x * tileSize.x) + (tileSize.x / 2.0);
			neighborCenter.y =
				origin.y + (neighborCell.y * tileSize.y) + (tileSize.y / 2.0);

			// Calculate line properties
			Vector2 lineVector{};
			lineVector.x = neighborCenter.x - center.x;
			lineVector.y = neighborCenter.y - center.y;

			// Calculate line length and angle
			double lineLength = std::sqrt(lineVector.x * lineVector.x +
										  lineVector.y * lineVector.y);
			double angle =
				std::atan2(lineVector.y, lineVector.x) * 180.0 / M_PI;

			// Calculate line center position
			Vector2 lineCenter{};
			lineCenter.x = center.x + (lineVector.x / 2.0);
			lineCenter.y = center.y + (lineVector.y / 2.0);

			// Create a thin rectangle to represent the line
			ShapeRenderCommand line;
			line.type = ShapeRenderType::Rectangle;
			line.position = lineCenter;
			line.size = {lineLength, lineThickness};
			line.rotationDegrees = angle;
			line.scale = {1.0, 1.0};
			line.color = debugGridLineColor;
			commands.push_back(line);
		}
	}

	// Third pass: Draw dots LAST (so they appear on top of lines)
	for ( const auto& [cell, center] : walkableCenters ) {
		// Draw a dot (small circle) at the center
		ShapeRenderCommand dot;
		dot.type = ShapeRenderType::Circle;
		dot.position = center;
		dot.radius = dotRadius;
		dot.rotationDegrees = 0.0;
		dot.scale = {1.0, 1.0};
		dot.color = debugWalkableDotColor;
		commands.push_back(dot);
	}

	for ( const auto& key : blockedCells ) {
		Vector2 cell{static_cast<double>(key.first),
					 static_cast<double>(key.second)};
		if ( !isValidCell(cell) ) {
			continue;
		}

		Vector2 tileCenter{};
		tileCenter.x = origin.x + (cell.x * tileSize.x) + (tileSize.x / 2.0);
		tileCenter.y = origin.y + (cell.y * tileSize.y) + (tileSize.y / 2.0);

		ShapeRenderCommand dot;
		dot.type = ShapeRenderType::Circle;
		dot.position = tileCenter;
		dot.radius = dotRadius;
		dot.rotationDegrees = 0.0;
		dot.scale = {1.0, 1.0};
		dot.color = debugBlockedDotColor;
		commands.push_back(dot);
	}

	return commands;
}

bool GridComponent::isReady() const
{
	return tilemapComponent != nullptr && tilemapComponent->isReady();
}
