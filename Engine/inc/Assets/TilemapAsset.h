#pragma once

#include "../Assets/Asset.h"
#include "../GameObject/Vector2.h"

#include <string>
#include <vector>

/**
 * @brief Asset that loads and stores tilemap data from CSV files.
 *
 * The CSV format should be:
 * - Each row represents a row of tiles
 * - Each column represents a column of tiles
 * - Values are integers representing tile IDs (0 = empty/air)
 * - Comma-separated values
 *
 * Example:
 * 1,1,1,1,1
 * 1,0,0,0,1
 * 1,0,0,0,1
 * 1,1,1,1,1
 */
class TilemapAsset: public Asset
{
   public:
	TilemapAsset();
	~TilemapAsset() override = default;

	bool load(const std::string& filepath) override;
	bool unLoad() override;
	bool isLoaded() const override;

	/**
	 * @brief Get the width of the tilemap in tiles.
	 */
	int getWidth() const;

	/**
	 * @brief Get the height of the tilemap in tiles.
	 */
	int getHeight() const;

	/**
	 * @brief Get the tile ID at the given grid coordinates.
	 * @param x Column index (0-based)
	 * @param y Row index (0-based)
	 * @return Tile ID, or 0 if coordinates are out of bounds
	 */
	int getTile(int x, int y) const;

	/**
	 * @brief Get the tile ID at the given grid coordinates.
	 * @param cell Grid coordinates {x, y}
	 * @return Tile ID, or 0 if coordinates are out of bounds
	 */
	int getTile(Vector2 cell) const;

	/**
	 * @brief Check if a tile exists (non-zero) at the given coordinates.
	 */
	bool hasTile(int x, int y) const;

	/**
	 * @brief Get all tile data as a flat array (row-major order).
	 */
	const std::vector<int>& getTiles() const;

   private:
	std::vector<int> tiles;
	int width = 0;
	int height = 0;
	bool loaded = false;
};
