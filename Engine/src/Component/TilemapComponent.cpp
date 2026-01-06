#include "Component/TilemapComponent.h"

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

	// Build a render command for each non-empty tile
	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			int tileId = tilemapAsset->getTile(x, y);

			// Calculate world position of this tile (top-left corner)
			Vector2 tileWorldPos{0.0, 0.0};
			tileWorldPos.x = origin.x + (x * tileSize.x);
			tileWorldPos.y = origin.y + (y * tileSize.y);

			// Center the rectangle at the tile position
			Vector2 tileCenter{0.0, 0.0};
			tileCenter.x = tileWorldPos.x + (tileSize.x / 2.0);
			tileCenter.y = tileWorldPos.y + (tileSize.y / 2.0);

			RenderCommand command;
			command.type = RenderCommandType::Rectangle;
			command.position = tileCenter;
			command.size = tileSize;
			command.rotationDegrees = rotation;
			command.scale = worldScale;
			command.color = getTileColor(tileId);
			command.layer = layer;
			command.orderInLayer = orderInLayer;
			queue.push(command);
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
	for (const auto& [tileId, color] : tileColors)
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

	// Collidable tile IDs
	uint32_t colliderCount = static_cast<uint32_t>(collidableTileIds.size());
	archive.process(colliderCount);
	for (int tileId : collidableTileIds)
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
	for (uint32_t i = 0; i < colorCount; ++i)
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

	// Collidable tile IDs
	uint32_t colliderCount = 0;
	archive.process(colliderCount);
	collidableTileIds.clear();
	for (uint32_t i = 0; i < colliderCount; ++i)
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
}
