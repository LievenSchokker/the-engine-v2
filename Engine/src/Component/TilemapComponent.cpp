#include "Component/TilemapComponent.h"

#include "Assets/IImage.h"
#include "Assets/TilemapAsset.h"
#include "Component/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector2Utils.h"

#include <cmath>
#include <iostream>

void TilemapComponent::setTilemapAsset(TilemapAsset* asset)
{
	tilemapAsset = asset;
}

void TilemapComponent::setTileSize(Vector2 newTileSize)
{
	tileSize = newTileSize;
}

Vector2 TilemapComponent::getTileSize() const
{
	return tileSize;
}

void TilemapComponent::setTileColor(int tileId, const Color& color)
{
	tileColors[tileId] = color;
}

Color TilemapComponent::getTileColor(int tileId) const
{
	auto it = tileColors.find(tileId);
	if ( it != tileColors.end() )
	{
		return it->second;
	}
	else
	{
		std::cout << "[TilemapComponent] Tile color not set for tile ID: "
				  << tileId << ", returning default white" << std::endl;
		return Color::white();
	}
}

void TilemapComponent::setTileset(IImage* image)
{
	tilesetImage = image;
}

IImage* TilemapComponent::getTileset() const
{
	return tilesetImage;
}

void TilemapComponent::setTilesetFrameSize(Vector2 frameSizePixels)
{
	tilesetFrameSize = frameSizePixels;
}

Vector2 TilemapComponent::getTilesetFrameSize() const
{
	return tilesetFrameSize;
}

void TilemapComponent::setTileSprite(int tileId, int frameIndex,
									 const Color& tint)
{
	tileSprites[tileId] = TileSprite{frameIndex, tint};
}

void TilemapComponent::setTileSprite(int tileId, int row, int col,
									 const Color& tint)
{
	if ( tilesetImage == nullptr || !tilesetImage->isLoaded() )
	{
		return;
	}

	const bool hasExplicitFrameSize =
		tilesetFrameSize.x > 0.0 && tilesetFrameSize.y > 0.0;
	const Vector2 frameSizePixels =
		hasExplicitFrameSize ? tilesetFrameSize : tileSize;
	const int frameWidth = static_cast<int>(std::round(frameSizePixels.x));
	const int frameHeight = static_cast<int>(std::round(frameSizePixels.y));
	if ( frameWidth <= 0 || frameHeight <= 0 )
	{
		return;
	}

	const int columns = tilesetImage->getWidth() / frameWidth;
	const int rows = tilesetImage->getHeight() / frameHeight;
	if ( columns <= 0 || rows <= 0 )
	{
		return;
	}

	if ( row < 0 || col < 0 || row >= rows || col >= columns )
	{
		return;
	}

	const int frameIndex = (row * columns) + col;
	setTileSprite(tileId, frameIndex, tint);
}

void TilemapComponent::setTileSprite(int tileId, IImage* image, int frameIndex,
									 const Color& tint)
{
	tilesetImage = image;
	setTileSprite(tileId, frameIndex, tint);
}

bool TilemapComponent::hasTileSprite(int tileId) const
{
	return tileSprites.contains(tileId);
}

const TilemapComponent::TileSprite* TilemapComponent::getTileSprite(
	int tileId) const
{
	auto it = tileSprites.find(tileId);
	if ( it != tileSprites.end() )
	{
		return &it->second;
	}
	return nullptr;
}

void TilemapComponent::setTileLayer(int tileId, uint8_t layer,
									int8_t orderInLayer)
{
	tileLayerOverrides[tileId] = TileLayer{layer, orderInLayer};
}

bool TilemapComponent::hasTileLayer(int tileId) const
{
	return tileLayerOverrides.contains(tileId);
}

const TilemapComponent::TileLayer* TilemapComponent::getTileLayer(
	int tileId) const
{
	auto it = tileLayerOverrides.find(tileId);
	if ( it != tileLayerOverrides.end() )
	{
		return &it->second;
	}
	return nullptr;
}

void TilemapComponent::setTileCollider(int tileId, bool enabled)
{
	if ( enabled )
	{
		collidableTileIds.insert(tileId);
	}
	else
	{
		collidableTileIds.erase(tileId);
	}
}

bool TilemapComponent::hasTileCollider(int tileId) const
{
	return collidableTileIds.contains(tileId);
}

const std::unordered_set<int>& TilemapComponent::getCollidableTileIds() const
{
	return collidableTileIds;
}

void TilemapComponent::fillRenderQueue(IRenderQueueWriter& queue) const
{
	if ( tilemapAsset == nullptr || !tilemapAsset->isLoaded() )
	{
		return;
	}

	const Transform* transform = getTransform();
	if ( transform == nullptr )
	{
		return;
	}

	// Use world transforms to respect parent-child hierarchy
	const Vector2 origin = transform->getWorldPosition();
	const double rotation = transform->getWorldRotation();
	const Vector2 worldScale =
		Vector2Utils::sanitizeScale(transform->getWorldScale());
	const int width = tilemapAsset->getWidth();
	const int height = tilemapAsset->getHeight();
	const bool spritesReady =
		tilesetImage != nullptr && tilesetImage->isLoaded();
	int frameWidth = 0;
	int frameHeight = 0;
	int columns = 0;
	int rows = 0;
	int totalFrames = 0;
	if ( spritesReady )
	{
		// Frame size is in image pixel units; by default we used tileSize for
		// this, but that couples world scale to spritesheet slicing.
		const bool hasExplicitFrameSize =
			tilesetFrameSize.x > 0.0 && tilesetFrameSize.y > 0.0;
		const Vector2 frameSizePixels =
			hasExplicitFrameSize ? tilesetFrameSize : tileSize;

		frameWidth = static_cast<int>(std::round(frameSizePixels.x));
		frameHeight = static_cast<int>(std::round(frameSizePixels.y));
		if ( frameWidth > 0 && frameHeight > 0 )
		{
			columns = tilesetImage->getWidth() / frameWidth;
			rows = tilesetImage->getHeight() / frameHeight;
			totalFrames = columns * rows;
		}
	}
	const bool canRenderSprites =
		spritesReady && columns > 0 && rows > 0 && totalFrames > 0;

	// Build a render command for each non-empty tile
	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			int tileId = tilemapAsset->getTile(x, y);
			uint8_t tileLayer = layer;
			int8_t tileOrder = orderInLayer;
			auto layerOverride = tileLayerOverrides.find(tileId);
			if ( layerOverride != tileLayerOverrides.end() )
			{
				tileLayer = layerOverride->second.layer;
				tileOrder = layerOverride->second.orderInLayer;
			}

			auto colorIt = tileColors.find(tileId);
			const bool hasColor = colorIt != tileColors.end();

			// Calculate world position of this tile (top-left corner)
			Vector2 tileWorldPos{0.0, 0.0};
			tileWorldPos.x = origin.x + (x * tileSize.x);
			tileWorldPos.y = origin.y + (y * tileSize.y);

			// Center the rectangle at the tile position
			Vector2 tileCenter{0.0, 0.0};
			tileCenter.x = tileWorldPos.x + (tileSize.x / 2.0);
			tileCenter.y = tileWorldPos.y + (tileSize.y / 2.0);

			const TileSprite* sprite = nullptr;
			int spriteRow = 0;
			int spriteCol = 0;
			if ( canRenderSprites )
			{
				auto spriteIt = tileSprites.find(tileId);
				if ( spriteIt != tileSprites.end() )
				{
					const TileSprite& spriteRef = spriteIt->second;
					if ( spriteRef.frameIndex >= 0 &&
						 spriteRef.frameIndex < totalFrames )
					{
						spriteRow = spriteRef.frameIndex / columns;
						spriteCol = spriteRef.frameIndex % columns;
						sprite = &spriteRef;
					}
				}
			}

			if ( !hasColor && sprite == nullptr )
			{
				continue;
			}

			const uint8_t baseOrder = static_cast<uint8_t>(tileOrder);
			uint8_t backgroundOrder = baseOrder;
			uint8_t spriteOrder = baseOrder;
			if ( hasColor && sprite != nullptr )
			{
				if ( baseOrder < 255 )
				{
					spriteOrder = static_cast<uint8_t>(baseOrder + 1);
				}
				else if ( baseOrder > 0 )
				{
					backgroundOrder = static_cast<uint8_t>(baseOrder - 1);
				}
			}

			if ( hasColor )
			{
				RenderCommand command;
				command.type = RenderCommandType::Rectangle;
				command.position = tileCenter;
				command.size = tileSize;
				command.rotationDegrees = rotation;
				command.scale = worldScale;
				command.color = colorIt->second;
				command.layer = tileLayer;
				command.orderInLayer = backgroundOrder;
				queue.push(command);
			}

			if ( sprite != nullptr )
			{
				RenderCommand command;
				command.type = RenderCommandType::Sprite;
				command.sprite = tilesetImage;
				command.srcRect =
					Rect(spriteCol * frameWidth, spriteRow * frameHeight,
						 frameWidth, frameHeight);
				command.position = tileCenter;
				command.size = tileSize;
				command.rotationDegrees = rotation;
				command.scale = worldScale;
				command.tint = sprite->tint;
				command.layer = tileLayer;
				command.orderInLayer = spriteOrder;
				queue.push(command);
			}
		}
	}
}

Vector2 TilemapComponent::worldToCell(Vector2 worldPos) const
{
	if ( tilemapAsset == nullptr || !tilemapAsset->isLoaded() )
	{
		return {0.0, 0.0};
	}

	const Transform* transform = getTransform();
	if ( transform == nullptr )
	{
		return {0.0, 0.0};
	}

	const Vector2 origin = transform->getWorldPosition();

	// Convert world position to grid coordinates
	Vector2 relativePos{0.0, 0.0};
	relativePos.x = worldPos.x - origin.x;
	relativePos.y = worldPos.y - origin.y;

	Vector2 cell{0.0, 0.0};
	cell.x = std::floor(relativePos.x / tileSize.x);
	cell.y = std::floor(relativePos.y / tileSize.y);

	return cell;
}

Vector2 TilemapComponent::cellToWorld(Vector2 cell) const
{
	if ( tilemapAsset == nullptr || !tilemapAsset->isLoaded() )
	{
		return {0.0, 0.0};
	}

	const Transform* transform = getTransform();
	if ( transform == nullptr )
	{
		return {0.0, 0.0};
	}

	const Vector2 origin = transform->getWorldPosition();

	// Convert grid coordinates to world position (center of tile)
	Vector2 worldPos{0.0, 0.0};
	worldPos.x = origin.x + (cell.x * tileSize.x) + (tileSize.x / 2.0);
	worldPos.y = origin.y + (cell.y * tileSize.y) + (tileSize.y / 2.0);

	return worldPos;
}

int TilemapComponent::getTileAt(Vector2 cell) const
{
	if ( tilemapAsset == nullptr || !tilemapAsset->isLoaded() )
	{
		return 0;
	}

	return tilemapAsset->getTile(static_cast<int>(cell.x),
								 static_cast<int>(cell.y));
}

int TilemapComponent::getTileAtWorld(Vector2 worldPos) const
{
	Vector2 cell = worldToCell(worldPos);
	return getTileAt(cell);
}

bool TilemapComponent::hasTileAt(Vector2 cell) const
{
	return getTileAt(cell) != 0;
}

int TilemapComponent::getGridWidth() const
{
	if ( tilemapAsset == nullptr )
	{
		return 0;
	}
	return tilemapAsset->getWidth();
}

int TilemapComponent::getGridHeight() const
{
	if ( tilemapAsset == nullptr )
	{
		return 0;
	}
	return tilemapAsset->getHeight();
}

bool TilemapComponent::isReady() const
{
	return tilemapAsset != nullptr && tilemapAsset->isLoaded();
}

void TilemapComponent::setLayer(uint8_t l)
{
	layer = l;
}

void TilemapComponent::setOrderInLayer(int8_t order)
{
	orderInLayer = order;
}

void TilemapComponent::serialize(WriteArchive& archive) const
{
	// Tile size
	float tileSizeX = tileSize.x;
	float tileSizeY = tileSize.y;
	archive.process(tileSizeX);
	archive.process(tileSizeY);

	// Tile colors
	uint32_t colorCount = static_cast<uint32_t>(tileColors.size());
	archive.process(colorCount);
	for ( const auto& [tileId, color] : tileColors )
	{
		int id = tileId;
		uint8_t r = color.r;
		uint8_t g = color.g;
		uint8_t b = color.b;
		uint8_t a = color.a;
		archive.process(id);
		archive.process(r);
		archive.process(g);
		archive.process(b);
		archive.process(a);
	}

	// Tile sprites
	uint32_t spriteCount = static_cast<uint32_t>(tileSprites.size());
	archive.process(spriteCount);
	for ( const auto& [tileId, sprite] : tileSprites )
	{
		int id = tileId;
		int frameIndex = sprite.frameIndex;
		uint8_t r = sprite.tint.r;
		uint8_t g = sprite.tint.g;
		uint8_t b = sprite.tint.b;
		uint8_t a = sprite.tint.a;
		archive.process(id);
		archive.process(frameIndex);
		archive.process(r);
		archive.process(g);
		archive.process(b);
		archive.process(a);
	}

	// Tile layer overrides
	uint32_t layerCount = static_cast<uint32_t>(tileLayerOverrides.size());
	archive.process(layerCount);
	for ( const auto& [tileId, layerOverride] : tileLayerOverrides )
	{
		int id = tileId;
		uint8_t lay = layerOverride.layer;
		int8_t order = layerOverride.orderInLayer;
		archive.process(id);
		archive.process(lay);
		archive.process(order);
	}

	// Collidable tile IDs
	uint32_t colliderCount = static_cast<uint32_t>(collidableTileIds.size());
	archive.process(colliderCount);
	for ( int tileId : collidableTileIds )
	{
		int id = tileId;
		archive.process(id);
	}

	// Layer info
	uint8_t lay = layer;
	int8_t order = orderInLayer;
	archive.process(lay);
	archive.process(order);
}

void TilemapComponent::deserialize(ReadArchive& archive)
{
	// Tile size
	archive.process(tileSize.x);
	archive.process(tileSize.y);

	// Tile colors
	uint32_t colorCount;
	archive.process(colorCount);
	tileColors.clear();
	for ( uint32_t i = 0; i < colorCount; ++i )
	{
		int tileId;
		uint8_t r, g, b, a;
		archive.process(tileId);
		archive.process(r);
		archive.process(g);
		archive.process(b);
		archive.process(a);
		tileColors[tileId] = Color(r, g, b, a);
	}

	// Tile sprites
	uint32_t spriteCount = 0;
	archive.process(spriteCount);
	tileSprites.clear();
	for ( uint32_t i = 0; i < spriteCount; ++i )
	{
		int tileId;
		int frameIndex;
		uint8_t r, g, b, a;
		archive.process(tileId);
		archive.process(frameIndex);
		archive.process(r);
		archive.process(g);
		archive.process(b);
		archive.process(a);
		tileSprites[tileId] = TileSprite{frameIndex, Color(r, g, b, a)};
	}

	// Tile layer overrides
	uint32_t layerCount = 0;
	archive.process(layerCount);
	tileLayerOverrides.clear();
	for ( uint32_t i = 0; i < layerCount; ++i )
	{
		int tileId;
		uint8_t lay;
		int8_t order;
		archive.process(tileId);
		archive.process(lay);
		archive.process(order);
		tileLayerOverrides[tileId] = TileLayer{lay, order};
	}

	// Collidable tile IDs
	uint32_t colliderCount = 0;
	archive.process(colliderCount);
	collidableTileIds.clear();
	for ( uint32_t i = 0; i < colliderCount; ++i )
	{
		int tileId;
		archive.process(tileId);
		collidableTileIds.insert(tileId);
	}

	// Layer info
	archive.process(layer);
	archive.process(orderInLayer);

	// Note: tilemapAsset is runtime reference
	// Must be set via setTilemapAsset() after instantiation
	// Note: tilesetImage is runtime reference
	// Must be set via setTileset() after instantiation
	tilesetImage = nullptr;
}
