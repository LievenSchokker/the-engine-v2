#pragma once

#include "../GameObject/Vector2.h"
#include "../Rendering/Color.h"
#include "../Rendering/RenderQueue.h"
#include "Component.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

class TilemapComponent;

/**
 * @brief Hash function for cell coordinates (x, y) stored as std::pair<int,
 * int>.
 *
 * Needed because std::unordered_set doesn't know how to hash pairs by default.
 * This allows us to use cell coordinates as keys in the blockedCells set.
 */
struct CellKeyHash {
	size_t operator()(const std::pair<int, int>& key) const noexcept
	{
		return std::hash<int>()(key.first) ^
			   (std::hash<int>()(key.second) << 1);
	}
};

/**
 * @brief Component that provides grid-based pathfinding queries for AI.
 *
 * This component wraps a TilemapComponent and provides:
 * - Walkability queries (is a cell walkable?)
 * - Neighbor enumeration (get adjacent cells)
 * - Distance calculations (for pathfinding heuristics)
 * - Debug visualization (grid lines, walkable cells)
 *
 * Designed for future AI pathfinding algorithms (A*, Dijkstra, etc.)
 */
class GridComponent: public Component
{
   public:
	GridComponent();
	~GridComponent() override = default;

	/**
	 * @brief Set the tilemap component to use for grid queries.
	 * @param tilemap Pointer to a TilemapComponent
	 */
	void setTilemapComponent(TilemapComponent* tilemap);

	/**
	 * @brief Check if a cell is walkable (tile ID == 0 means walkable).
	 * @param cell Grid coordinates {x, y}
	 * @return true if the cell is walkable, false if blocked or out of bounds
	 */
	bool isWalkable(Vector2 cell) const;

	/**
	 * @brief Check if a world position is walkable.
	 * @param worldPos World position
	 * @return true if walkable, false otherwise
	 */
	bool isWalkableAtWorld(Vector2 worldPos) const;

	/**
	 * @brief Override which tile IDs are considered walkable.
	 * @param tileIds Collection of tile IDs that should be treated as walkable.
	 */
	void setWalkableTileIds(const std::vector<int>& tileIds);

	/**
	 * @brief Mark an additional tile ID as walkable.
	 */
	void addWalkableTileId(int tileId);

	/**
	 * @brief Clear any custom walkable tile IDs.
	 */
	void clearWalkableTileIds();

	/**
	 * @brief Dynamically mark a grid cell as blocked.
	 */
	void blockCell(Vector2 cell);

	/**
	 * @brief Remove a previously blocked cell.
	 */
	void unblockCell(Vector2 cell);

	/**
	 * @brief Check if a cell is blocked dynamically.
	 */
	bool isCellBlocked(Vector2 cell) const;

	/**
	 * @brief Clear all dynamic blockers.
	 */
	void clearBlockedCells();

	/**
	 * @brief Set the movement cost/weight for a specific tile ID.
	 * @param tileId The tile ID to set weight for
	 * @param weight Movement cost (1.0 = normal, higher = slower, lower =
	 * faster)
	 */
	void setTileWeight(int tileId, double weight);

	/**
	 * @brief Get the movement cost/weight for a specific tile ID.
	 * @param tileId The tile ID
	 * @return Movement cost, or 1.0 if not set
	 */
	double getTileWeight(int tileId) const;

	/**
	 * @brief Get the movement cost/weight for a cell.
	 * @param cell Grid coordinates {x, y}
	 * @return Movement cost for the tile at this cell, or 1.0 if not set
	 */
	double getCellWeight(Vector2 cell) const;

	/**
	 * @brief Get all walkable neighboring cells (4-directional: up, down, left,
	 * right).
	 * @param cell Grid coordinates {x, y}
	 * @return Vector of walkable neighbor cells
	 */
	std::vector<Vector2> getWalkableNeighbors(Vector2 cell) const;

	/**
	 * @brief Get all neighboring cells including diagonals (8-directional).
	 * @param cell Grid coordinates {x, y}
	 * @param includeDiagonals If true, includes diagonal neighbors
	 * @return Vector of walkable neighbor cells
	 */
	std::vector<Vector2> getNeighbors(Vector2 cell,
									  bool includeDiagonals = false) const;

	/**
	 * @brief Calculate Manhattan distance between two cells (for pathfinding).
	 * @param from Starting cell
	 * @param to Target cell
	 * @return Manhattan distance (|dx| + |dy|)
	 */
	static double manhattanDistance(Vector2 from, Vector2 to);

	/**
	 * @brief Calculate Euclidean distance between two cells.
	 * @param from Starting cell
	 * @param to Target cell
	 * @return Euclidean distance
	 */
	static double euclideanDistance(Vector2 from, Vector2 to);

	/**
	 * @brief Check if a cell is within grid bounds.
	 * @param cell Grid coordinates {x, y}
	 * @return true if within bounds, false otherwise
	 */
	bool isValidCell(Vector2 cell) const;
	bool isTileWalkable(int tileId) const;

	/**
	 * @brief Get grid width.
	 */
	int getGridWidth() const;

	/**
	 * @brief Get grid height.
	 */
	int getGridHeight() const;

	/**
	 * @brief Enable or disable debug visualization.
	 * @param enabled If true, grid will be rendered for debugging
	 */
	void setDebugRenderEnabled(bool enabled);

	/**
	 * @brief Check if debug rendering is enabled.
	 */
	bool isDebugRenderEnabled() const;
	bool isDebugShowDiagonalLinks() const;

	/**
	 * @brief Set the color for walkable cells in debug rendering.
	 * @param color Color to use for walkable cells
	 */
	void setDebugWalkableColor(const Color& color);

	/**
	 * @brief Set the color for blocked cells in debug rendering.
	 * @param color Color to use for blocked cells
	 */
	void setDebugBlockedColor(const Color& color);

	/**
	 * @brief Set the color for grid lines in debug rendering.
	 * @param color Color to use for grid lines
	 */
	void setDebugGridLineColor(const Color& color);

	/**
	 * @brief Enable or disable diagonal debug links between walkable tiles.
	 */
	void setDebugShowDiagonalLinks(bool enabled);

	/**
	 * @brief Build render commands for debug visualization.
	 * @return Vector of render commands for grid visualization
	 */
	std::vector<ShapeRenderCommand> buildDebugRenderCommands() const;

	/**
	 * @brief Check if the grid is ready for queries.
	 */
	bool isReady() const;

   private:
	TilemapComponent* tilemapComponent = nullptr;
	bool debugRenderEnabled = false;
	Color debugWalkableDotColor = Color::green();  // Solid green for dots
	Color debugBlockedDotColor = Color::red();	   // Solid red for blocked dots
	Color debugGridLineColor =
		Color::darkGray();	// Dark gray for debug grid lines
	bool debugShowDiagonalLinks = false;
	std::unordered_set<int> walkableTileIds{0};
	std::unordered_set<std::pair<int, int>, CellKeyHash> blockedCells;
	std::unordered_map<int, double>
		tileWeights;  // Map tile ID to movement cost

	static std::pair<int, int> cellKey(Vector2 cell);
};
