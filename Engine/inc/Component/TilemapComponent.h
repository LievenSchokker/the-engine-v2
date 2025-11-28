#pragma once

#include "GameObject/Vector2.h"
#include "Rendering/Color.h"
#include "Rendering/RenderQueue.h"
#include "Component.h"

#include <unordered_map>
#include <vector>

class TilemapAsset;

/**
 * @brief Component that renders a tilemap and provides grid query
 * functionality.
 *
 * This component:
 * - Renders tiles as colored rectangles (one per non-empty tile)
 * - Provides coordinate conversion between world space and grid space
 * - Can be queried for tile information at specific grid coordinates
 *
 * The tilemap is positioned based on the GameObject's Transform.
 */
class TilemapComponent: public Component
{
   public:
	TilemapComponent();
	~TilemapComponent() override = default;

	/**
	 * @brief Set the tilemap asset to use.
	 * @param asset Pointer to a loaded TilemapAsset
	 */
	void setTilemapAsset(TilemapAsset* asset);

	/**
	 * @brief Set the size of each tile in world units.
	 * @param tileSize Size of a single tile (width, height)
	 */
	void setTileSize(Vector2 tileSize);

	/**
	 * @brief Get the tile size in world units.
	 */
	Vector2 getTileSize() const;

	/**
	 * @brief Set the color for a specific tile ID.
	 * @param tileId The tile ID to set color for
	 * @param color The color to use
	 */
	void setTileColor(int tileId, const Color& color);

	/**
	 * @brief Get the color for a specific tile ID.
	 * @param tileId The tile ID
	 * @return The color, or white if not set
	 */
	Color getTileColor(int tileId) const;

	/**
	 * @brief Build render commands for all visible tiles.
	 * @return Vector of render commands, or empty if no tilemap is loaded
	 */
	std::vector<ShapeRenderCommand> buildRenderCommands() const;

	/**
	 * @brief Convert world coordinates to grid cell coordinates.
	 * @param worldPos World position
	 * @return Grid cell coordinates {x, y}
	 */
	Vector2 worldToCell(Vector2 worldPos) const;

	/**
	 * @brief Convert grid cell coordinates to world position (center of tile).
	 * @param cell Grid cell coordinates {x, y}
	 * @return World position at the center of the tile
	 */
	Vector2 cellToWorld(Vector2 cell) const;

	/**
	 * @brief Get the tile ID at the given grid coordinates.
	 * @param cell Grid coordinates {x, y}
	 * @return Tile ID, or 0 if out of bounds or no tilemap loaded
	 */
	int getTileAt(Vector2 cell) const;

	/**
	 * @brief Get the tile ID at the given world coordinates.
	 * @param worldPos World position
	 * @return Tile ID, or 0 if out of bounds or no tilemap loaded
	 */
	int getTileAtWorld(Vector2 worldPos) const;

	/**
	 * @brief Check if a tile exists at the given grid coordinates.
	 */
	bool hasTileAt(Vector2 cell) const;

	/**
	 * @brief Get the width of the tilemap in tiles.
	 */
	int getGridWidth() const;

	/**
	 * @brief Get the height of the tilemap in tiles.
	 */
	int getGridHeight() const;

	/**
	 * @brief Check if the tilemap is loaded and ready.
	 */
	bool isReady() const;

   private:
	TilemapAsset* tilemapAsset = nullptr;
	Vector2 tileSize{32.0, 32.0};				// Default tile size
	std::unordered_map<int, Color> tileColors;	// Map tile ID to color
};
