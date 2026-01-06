#include "Component/GridComponent.h"

#include "Component/TilemapComponent.h"
#include "Component/Transform.h"

#include <algorithm>
#include <cmath>

#if defined WIN32
    #include <corecrt_math_defines.h>
#endif


GridComponent::GridComponent()
{
}

void GridComponent::setTilemapComponent(TilemapComponent* tilemap)
{
	// Validate that tilemap is on the same GameObject (if both exist)
	if (tilemap != nullptr && getGameObject() != nullptr &&
	    tilemap->getGameObject() != nullptr &&
	    tilemap->getGameObject() != getGameObject())
	{
		// Warning: TilemapComponent is on a different GameObject
		// This is allowed but may cause issues if the other GameObject is
		// destroyed. For safety, prefer using a TilemapComponent on the same
		// GameObject
	}

	tilemapComponent = tilemap;
}

bool GridComponent::isWalkable(Vector2 cell) const
{
	if (tilemapComponent == nullptr || !tilemapComponent->isReady())
	{
		return false;
	}

	if (!isValidCell(cell))
	{
		return false;
	}

	if (isCellBlocked(cell))
	{
		return false;
	}

	// Tile ID 0 means empty/air = walkable
	int tileId = tilemapComponent->getTileAt(cell);
	return isTileWalkable(tileId);
}

bool GridComponent::isWalkableAtWorld(Vector2 worldPos) const
{
	if (tilemapComponent == nullptr || !tilemapComponent->isReady())
	{
		return false;
	}

	Vector2 cell = tilemapComponent->worldToCell(worldPos);
	return isWalkable(cell);
}

void GridComponent::setWalkableTileIds(const std::vector<int>& tileIds)
{
	walkableTileIds.clear();
	for (int id : tileIds)
	{
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
	if (it != tileWeights.end())
	{
		return it->second;
	}
	return 1.0; // Default weight
}

double GridComponent::getCellWeight(Vector2 cell) const
{
	if (tilemapComponent == nullptr || !tilemapComponent->isReady())
	{
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
	for (const auto& neighbor : neighbors)
	{
		if (isWalkable(neighbor))
		{
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
	neighbors.push_back({cell.x, cell.y - 1}); // Up
	neighbors.push_back({cell.x, cell.y + 1}); // Down
	neighbors.push_back({cell.x - 1, cell.y}); // Left
	neighbors.push_back({cell.x + 1, cell.y}); // Right

	// 8-directional neighbors (including diagonals)
	if (includeDiagonals)
	{
		neighbors.push_back({cell.x - 1, cell.y - 1}); // Up-Left
		neighbors.push_back({cell.x + 1, cell.y - 1}); // Up-Right
		neighbors.push_back({cell.x - 1, cell.y + 1}); // Down-Left
		neighbors.push_back({cell.x + 1, cell.y + 1}); // Down-Right
	}

	// Filter out invalid cells
	std::vector<Vector2> validNeighbors;
	for (const auto& neighbor : neighbors)
	{
		if (isValidCell(neighbor))
		{
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
	if (tilemapComponent == nullptr || !tilemapComponent->isReady())
	{
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
	if (tilemapComponent == nullptr)
	{
		return 0;
	}
	return tilemapComponent->getGridWidth();
}

int GridComponent::getGridHeight() const
{
	if (tilemapComponent == nullptr)
	{
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

void GridComponent::fillRenderQueue(IRenderQueueWriter& queue) const
{
	if (!debugRenderEnabled || tilemapComponent == nullptr ||
	    !tilemapComponent->isReady())
	{
		return;
	}

	const Transform* transform = getTransform();
	if (transform == nullptr)
	{
		// Try to get transform from tilemap's GameObject
		if (tilemapComponent->getGameObject() != nullptr)
		{
			transform = tilemapComponent->getGameObject()->getTransform();
		}
		if (transform == nullptr)
		{
			return;
		}
	}

	const Vector2 origin = transform->getPosition();
	const Vector2 tileSize = tilemapComponent->getTileSize();
	const int width = tilemapComponent->getGridWidth();
	const int height = tilemapComponent->getGridHeight();

	// Calculate dot size (small circle at center of walkable tiles)
	const double dotRadius =
		std::min(tileSize.x, tileSize.y) * 0.1; // 10% of tile size

	// Calculate line thickness (for connections between walkable tiles)
	const double lineThickness =
		std::min(tileSize.x, tileSize.y) * 0.05; // 5% of tile size

	// First pass: Collect walkable tile centers (don't draw yet)
	std::vector<std::pair<Vector2, Vector2>>
		walkableCenters; // (cell, worldCenter)

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Vector2 cell{static_cast<float>(x), static_cast<float>(y)};

			// Only render on walkable tiles (tile ID == 0)
			if (!isWalkable(cell))
			{
				continue;
			}

			// Calculate tile center position
			Vector2 tileCenter{0.0, 0.0};
			tileCenter.x = origin.x + (static_cast<float>(x) * tileSize.x) +
			               (tileSize.x / 2.0f);
			tileCenter.y = origin.y + (static_cast<float>(y) * tileSize.y) +
			               (tileSize.y / 2.0f);

			// Store center for line and dot drawing
			walkableCenters.push_back({cell, tileCenter});
		}
	}

	// Second pass: Draw lines FIRST (so they appear below dots)
	// Lines connecting walkable tiles to their walkable neighbors
	// Includes diagonals for visualization purposes
	for (const auto& [cell, center] : walkableCenters)
	{
		// Use diagonals for debug visualization when requested while
		// pathfinding logic elsewhere can still choose 4-directional neighbors
		// if needed
		std::vector<Vector2> neighbors =
			getNeighbors(cell, debugShowDiagonalLinks);

		for (const auto& neighborCell : neighbors)
		{
			// Only draw line if neighbor is also walkable
			if (!isWalkable(neighborCell))
			{
				continue;
			}

			// Find the world center of the neighbor
			Vector2 neighborCenter{0.0, 0.0};
			neighborCenter.x =
				origin.x + (neighborCell.x * tileSize.x) + (tileSize.x / 2.0f);
			neighborCenter.y =
				origin.y + (neighborCell.y * tileSize.y) + (tileSize.y / 2.0f);

			// Calculate line properties
			Vector2 lineVector{0.0, 0.0};
			lineVector.x = neighborCenter.x - center.x;
			lineVector.y = neighborCenter.y - center.y;

			// Calculate line length and angle
			double lineLength = std::sqrt(lineVector.x * lineVector.x +
										  lineVector.y * lineVector.y);
			double angle =
				std::atan2(lineVector.y, lineVector.x) * 180.0 / M_PI;

			// Calculate line center position
			Vector2 lineCenter{0.0, 0.0};
			lineCenter.x = center.x + (lineVector.x / 2.0f);
			lineCenter.y = center.y + (lineVector.y / 2.0f);

			// Create a thin rectangle to represent the line
			RenderCommand line;
			line.type = RenderCommandType::Rectangle;
			line.position = lineCenter;
			line.size = {static_cast<float>(lineLength),
			             static_cast<float>(lineThickness)};
			line.rotationDegrees = angle;
			line.scale = {1.0, 1.0};
			line.color = debugGridLineColor;
			line.layer = layer;
			line.orderInLayer = orderInLayer;
			queue.push(line);
		}
	}

	// Third pass: Draw dots LAST (so they appear on top of lines)
	for (const auto& [cell, center] : walkableCenters)
	{
		// Draw a dot (small circle) at the center
		RenderCommand dot;
		dot.type = RenderCommandType::Circle;
		dot.position = center;
		dot.radius = dotRadius;
		dot.rotationDegrees = 0.0;
		dot.scale = {1.0, 1.0};
		dot.color = debugWalkableDotColor;
		dot.layer = layer;
		dot.orderInLayer = orderInLayer;
		queue.push(dot);
	}

	for (const auto& key : blockedCells)
	{
		Vector2 cell{static_cast<float>(key.first),
		             static_cast<float>(key.second)};
		if (!isValidCell(cell))
		{
			continue;
		}

		Vector2 tileCenter{0.0, 0.0};
		tileCenter.x = origin.x + (cell.x * tileSize.x) + (tileSize.x / 2.0f);
		tileCenter.y = origin.y + (cell.y * tileSize.y) + (tileSize.y / 2.0f);

		RenderCommand dot;
		dot.type = RenderCommandType::Circle;
		dot.position = tileCenter;
		dot.radius = dotRadius;
		dot.rotationDegrees = 0.0;
		dot.scale = {1.0, 1.0};
		dot.color = debugBlockedDotColor;
		dot.layer = layer;
		dot.orderInLayer = orderInLayer;
		queue.push(dot);
	}
}

bool GridComponent::isReady() const
{
	return tilemapComponent != nullptr && tilemapComponent->isReady();
}

void GridComponent::setLayer(uint8_t l)
{
	layer = l;
}

void GridComponent::setOrderInLayer(int8_t order)
{
	orderInLayer = order;
}

void GridComponent::serialize(WriteArchive& archive) const
{
    // Debug settings
    bool debugEnabled = debugRenderEnabled;
    archive.process(debugEnabled);

    uint8_t walkR = debugWalkableDotColor.r;
    uint8_t walkG = debugWalkableDotColor.g;
    uint8_t walkB = debugWalkableDotColor.b;
    uint8_t walkA = debugWalkableDotColor.a;
    archive.process(walkR);
    archive.process(walkG);
    archive.process(walkB);
    archive.process(walkA);

    uint8_t blockR = debugBlockedDotColor.r;
    uint8_t blockG = debugBlockedDotColor.g;
    uint8_t blockB = debugBlockedDotColor.b;
    uint8_t blockA = debugBlockedDotColor.a;
    archive.process(blockR);
    archive.process(blockG);
    archive.process(blockB);
    archive.process(blockA);

    uint8_t lineR = debugGridLineColor.r;
    uint8_t lineG = debugGridLineColor.g;
    uint8_t lineB = debugGridLineColor.b;
    uint8_t lineA = debugGridLineColor.a;
    archive.process(lineR);
    archive.process(lineG);
    archive.process(lineB);
    archive.process(lineA);

    bool showDiag = debugShowDiagonalLinks;
    archive.process(showDiag);

    // Walkable tile IDs
    uint32_t walkableCount = static_cast<uint32_t>(walkableTileIds.size());
    archive.process(walkableCount);
    for (int tileId : walkableTileIds)
    {
        int id = tileId;
        archive.process(id);
    }

    // Tile weights
    uint32_t weightCount = static_cast<uint32_t>(tileWeights.size());
    archive.process(weightCount);
    for (const auto& [tileId, weight] : tileWeights)
    {
        int id = tileId;
        double w = weight;
        archive.process(id);
        archive.process(w);
    }

    // Layer info
    uint8_t lay = layer;
    int8_t order = orderInLayer;
    archive.process(lay);
    archive.process(order);
}

void GridComponent::deserialize(ReadArchive& archive)
{
    // Debug settings
    archive.process(debugRenderEnabled);

    uint8_t walkR, walkG, walkB, walkA;
    archive.process(walkR);
    archive.process(walkG);
    archive.process(walkB);
    archive.process(walkA);
    debugWalkableDotColor = Color(walkR, walkG, walkB, walkA);

    uint8_t blockR, blockG, blockB, blockA;
    archive.process(blockR);
    archive.process(blockG);
    archive.process(blockB);
    archive.process(blockA);
    debugBlockedDotColor = Color(blockR, blockG, blockB, blockA);

    uint8_t lineR, lineG, lineB, lineA;
    archive.process(lineR);
    archive.process(lineG);
    archive.process(lineB);
    archive.process(lineA);
    debugGridLineColor = Color(lineR, lineG, lineB, lineA);

    archive.process(debugShowDiagonalLinks);

    // Walkable tile IDs
    uint32_t walkableCount;
    archive.process(walkableCount);
    walkableTileIds.clear();
    for (uint32_t i = 0; i < walkableCount; ++i)
    {
        int tileId;
        archive.process(tileId);
        walkableTileIds.insert(tileId);
    }

    // Tile weights
    uint32_t weightCount;
    archive.process(weightCount);
    tileWeights.clear();
    for (uint32_t i = 0; i < weightCount; ++i)
    {
        int tileId;
        double weight;
        archive.process(tileId);
        archive.process(weight);
        tileWeights[tileId] = weight;
    }

    // Layer info
    archive.process(layer);
    archive.process(orderInLayer);
}