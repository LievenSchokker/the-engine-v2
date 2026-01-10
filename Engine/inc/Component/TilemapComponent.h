#pragma once

#include "BaseComponentTypes/RenderComponent.h"
#include "Math/Vector2.h"
#include "Networking/Serialization/RegistrationBase.h"
#include "Rendering/Color.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

class IImage;
class TilemapAsset;

/**
 * @brief Component that renders a tilemap and provides grid query
 * functionality.
 *
 * This component:
 * - Renders tiles as sprites when mapped, otherwise colored rectangles
 * - Provides coordinate conversion between world space and grid space
 * - Can be queried for tile information at specific grid coordinates
 *
 * The tilemap is positioned based on the GameObject's Transform.
 */
class TilemapComponent: public RenderComponent,
						RegistrationBase<TilemapComponent>
{
   public:
	TilemapComponent() : tileSize{32.0, 32.0} {};
	~TilemapComponent() override = default;

	static constexpr const char* name()
	{
		return "Tilemap";
	}

	const char* getName() const override
	{
		return name();
	}

	struct TileSprite
	{
		int frameIndex = 0;
		Color tint = Color::white();
	};

	struct TileOrderOverride
	{
		int8_t orderInLayer = 0;
	};

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
	 * @param color The color to use (rendered behind sprites when set)
	 */
	void setTileColor(int tileId, const Color& color);

	/**
	 * @brief Get the color for a specific tile ID.
	 * @param tileId The tile ID
	 * @return The color, or white if not set
	 */
	Color getTileColor(int tileId) const;

	/**
	 * @brief Set the tileset image used for tile sprites.
	 * @param image Pointer to a loaded tileset image
	 */
	void setTileset(IImage* image);

	/**
	 * @brief Get the tileset image used for tile sprites.
	 */
	IImage* getTileset() const;

	/**
	 * @brief Set the spritesheet frame size (in pixels) used when slicing the
	 * tileset image into frames.
	 *
	 * This is intentionally separate from `tileSize`:
	 * - `tileSize` controls the size of a tile in *world units*.
	 * - `tilesetFrameSize` controls the size of a frame in the *image*
	 * (pixels).
	 *
	 * If left unset (0,0), the component falls back to using `tileSize` as the
	 * frame size (legacy behavior).
	 */
	void setTilesetFrameSize(Vector2 frameSizePixels);

	/**
	 * @brief Get the configured tileset frame size (pixels). May be (0,0) if
	 * unset.
	 */
	Vector2 getTilesetFrameSize() const;

	/**
	 * @brief Set the sprite frame for a specific tile ID.
	 * @param tileId The tile ID to set sprite for
	 * @param frameIndex The frame index in the tileset
	 * @param tint Optional tint color (white for no tint)
	 */
	void setTileSprite(int tileId, int frameIndex,
					   const Color& tint = Color::white());

	/**
	 * @brief Set the sprite frame for a tile ID using tileset row/column.
	 * @param tileId The tile ID to set sprite for
	 * @param row The row index in the tileset (0-based)
	 * @param col The column index in the tileset (0-based)
	 * @param tint Optional tint color (white for no tint)
	 */
	void setTileSprite(int tileId, int row, int col,
					   const Color& tint = Color::white());

	/**
	 * @brief Set the shared tileset image and sprite frame for a tile ID.
	 * @param tileId The tile ID to set sprite for
	 * @param image Pointer to a loaded tileset image (shared)
	 * @param frameIndex The frame index in the tileset
	 * @param tint Optional tint color (white for no tint)
	 */
	void setTileSprite(int tileId, IImage* image, int frameIndex,
					   const Color& tint = Color::white());

	/**
	 * @brief Override render order within the GameObject layer for a tile ID.
	 * @param tileId The tile ID to set order for
	 * @param orderInLayer The order within the layer
	 */
	void setTileOrderOverride(int tileId, int8_t orderInLayer = 0);

	/**
	 * @brief Check if a tile ID has an order override.
	 */
	bool hasTileOrderOverride(int tileId) const;

	/**
	 * @brief Get the order override for a tile ID, or nullptr if missing.
	 */
	const TileOrderOverride* getTileOrderOverride(int tileId) const;

	/**
	 * @brief Check if a tile ID has a sprite mapping.
	 */
	bool hasTileSprite(int tileId) const;

	/**
	 * @brief Get the sprite mapping for a tile ID, or nullptr if missing.
	 */
	const TileSprite* getTileSprite(int tileId) const;

	/**
	 * @brief Enable or disable colliders for a specific tile ID.
	 * @param tileId The tile ID to set collider for
	 * @param enabled True to add colliders for this tile ID
	 */
	void setTileCollider(int tileId, bool enabled);

	/**
	 * @brief Check if a tile ID should spawn colliders.
	 */
	bool hasTileCollider(int tileId) const;

	/**
	 * @brief Retrieve all tile IDs that should spawn colliders.
	 */
	const std::unordered_set<int>& getCollidableTileIds() const;

	/**
	 * @brief Build render commands for all visible tiles.
	 * @return Vector of render commands, or empty if no tilemap is loaded
	 */
	void fillRenderQueue(IRenderQueueWriter& queue) const override;

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
	void setOrderInLayer(int8_t order);
	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;

   private:
	TilemapAsset* tilemapAsset = nullptr;
	IImage* tilesetImage = nullptr;
	Vector2 tileSize;
	Vector2 tilesetFrameSize{0.0, 0.0};
	std::unordered_map<int, Color> tileColors;
	std::unordered_map<int, TileSprite> tileSprites;
	std::unordered_map<int, TileOrderOverride> tileOrderOverrides;
	std::unordered_set<int> collidableTileIds;
	int8_t orderInLayer = 0;
};
