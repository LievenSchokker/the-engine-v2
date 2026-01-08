#include "Component/TilemapColliderComponent.h"

#include "Component/TilemapComponent.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Math/Vector2Utils.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"
#include "Scene/Scene.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace
{
size_t cellIndex(int x, int y, int width)
{
	return static_cast<size_t>(y * width + x);
}

struct RectSize
{
	int width = 0;
	int height = 0;
};

RectSize computeRectSize(const std::vector<uint8_t>& collidable,
                         const std::vector<uint8_t>& visited,
                         int startX,
                         int startY,
                         int width,
                         int height)
{
	RectSize rect{};
	while ( (startX + rect.width) < width )
	{
		const size_t rowIndex = cellIndex(startX + rect.width, startY, width);
		if ( collidable[rowIndex] == 0 || visited[rowIndex] == 1 )
		{
			break;
		}
		++rect.width;
	}

	rect.height = 1;
	bool canExpand = true;
	while ( (startY + rect.height) < height && canExpand )
	{
		for ( int dx = 0; dx < rect.width; ++dx )
		{
			const size_t rowIndex =
				cellIndex(startX + dx, startY + rect.height, width);
			if ( collidable[rowIndex] == 0 || visited[rowIndex] == 1 )
			{
				canExpand = false;
				break;
			}
		}

		if ( canExpand )
		{
			++rect.height;
		}
	}

	return rect;
}

void markVisited(std::vector<uint8_t>& visited,
                 int startX,
                 int startY,
                 int rectWidth,
                 int rectHeight,
                 int width)
{
	for ( int dy = 0; dy < rectHeight; ++dy )
	{
		for ( int dx = 0; dx < rectWidth; ++dx )
		{
			visited[cellIndex(startX + dx, startY + dy, width)] = 1;
		}
	}
}
} // namespace

struct TilemapColliderComponent::BakeContext
{
	TilemapComponent* tilemap = nullptr;
	GameObject* owner = nullptr;
	Scene* scene = nullptr;
	const Transform* transform = nullptr;
	int width = 0;
	int height = 0;
	Vector2 tileSize = Vector2::zero();
	Vector2 origin = Vector2::zero();
	double rotation = 0.0;
	Vector2 scaledTileSize = Vector2::zero();
};

void TilemapColliderComponent::setTilemapComponent(TilemapComponent* tilemap)
{
	tilemapComponent = tilemap;
}

TilemapComponent* TilemapColliderComponent::getTilemapComponent() const
{
	return tilemapComponent;
}

void TilemapColliderComponent::setAutoBake(bool enabled)
{
	autoBake = enabled;
}

bool TilemapColliderComponent::getAutoBake() const
{
	return autoBake;
}

bool TilemapColliderComponent::ensureTilemapReady()
{
	if ( tilemapComponent == nullptr )
	{
		tilemapComponent = getComponent<TilemapComponent>();
	}

	if ( tilemapComponent == nullptr || !tilemapComponent->isReady() )
	{
		std::cout
			<< "[TilemapColliderComponent] Tilemap not ready, skipping bake."
			<< std::endl;
		return false;
	}

	return true;
}

bool TilemapColliderComponent::buildBakeContext(BakeContext& context) const
{
	if ( tilemapComponent->getCollidableTileIds().empty() )
	{
		return false;
	}

	context.owner = getGameObject();
	if ( context.owner == nullptr )
	{
		return false;
	}

	context.scene = context.owner->getScene();
	if ( context.scene == nullptr )
	{
		return false;
	}

	context.transform = tilemapComponent->getTransform();
	if ( context.transform == nullptr )
	{
		return false;
	}

	context.width = tilemapComponent->getGridWidth();
	context.height = tilemapComponent->getGridHeight();
	if ( context.width <= 0 || context.height <= 0 )
	{
		return false;
	}

	context.tileSize = tilemapComponent->getTileSize();
	if ( context.tileSize.x <= 0.0f || context.tileSize.y <= 0.0f )
	{
		return false;
	}

	context.origin = context.transform->getWorldPosition();
	context.rotation = context.transform->getWorldRotation();
	const Vector2 worldScale =
		Vector2Utils::sanitizeScale(context.transform->getWorldScale());
	context.scaledTileSize = Vector2{
		static_cast<float>(std::abs(context.tileSize.x * worldScale.x)),
		static_cast<float>(std::abs(context.tileSize.y * worldScale.y))};

	return true;
}

void TilemapColliderComponent::populateCollidableMask(
	const BakeContext& context,
	std::vector<uint8_t>& collidable) const
{
	for ( int y = 0; y < context.height; ++y )
	{
		for ( int x = 0; x < context.width; ++x )
		{
			Vector2 cell{static_cast<float>(x), static_cast<float>(y)};
			const int tileId = tilemapComponent->getTileAt(cell);
			if ( tilemapComponent->hasTileCollider(tileId) )
			{
				collidable[cellIndex(x, y, context.width)] = 1;
			}
		}
	}
}

ObjectHandle TilemapColliderComponent::spawnCollider(const BakeContext& context,
                                                     int x,
                                                     int y,
                                                     int rectWidth,
                                                     int rectHeight,
                                                     int colliderIndex) const
{
	const Vector2 localSize{context.tileSize.x * rectWidth,
	                        context.tileSize.y * rectHeight};
	const Vector2 center{
		context.origin.x + (static_cast<float>(x) * context.tileSize.x) +
			(localSize.x * 0.5f),
		context.origin.y + (static_cast<float>(y) * context.tileSize.y) +
			(localSize.y * 0.5f)};
	const Vector2 colliderSize{context.scaledTileSize.x * rectWidth,
	                           context.scaledTileSize.y * rectHeight};

	auto colliderObject = std::make_unique<GameObject>();
	colliderObject->setName(context.owner->getName() + "_TileCollider_" +
	                        std::to_string(colliderIndex));
	colliderObject->setIsStatic(true);

	Transform* colliderTransform = colliderObject->getTransform();
	colliderTransform->setPosition(center);
	colliderTransform->setRotationAngle(context.rotation);

	auto* collider = colliderObject->addComponent<Collider>();
	collider->setRectangle(colliderSize);

	auto* rigidBody = colliderObject->addComponent<RigidBody>();
	rigidBody->makeStatic();

	return context.scene->addGameObject(std::move(colliderObject));
}

void TilemapColliderComponent::bakeColliders()
{
	clearColliders();

	if ( !ensureTilemapReady() )
	{
		return;
	}

	BakeContext context{};
	if ( !buildBakeContext(context) )
	{
		return;
	}

	const size_t cellCount = static_cast<size_t>(context.width) *
	                         static_cast<size_t>(context.height);
	std::vector<uint8_t> collidable(cellCount, 0);
	std::vector<uint8_t> visited(cellCount, 0);

	populateCollidableMask(context, collidable);

	int colliderIndex = 0;

	for ( int y = 0; y < context.height; ++y )
	{
		for ( int x = 0; x < context.width; ++x )
		{
			const size_t index = cellIndex(x, y, context.width);
			if ( collidable[index] == 0 || visited[index] == 1 )
			{
				continue;
			}

			const RectSize rect =
				computeRectSize(collidable, visited, x, y, context.width,
				                context.height);
			markVisited(visited, x, y, rect.width, rect.height, context.width);

			ObjectHandle handle =
				spawnCollider(context, x, y, rect.width, rect.height,
				              colliderIndex++);
			if ( handle.isValid() )
			{
				colliderHandles.push_back(handle);
			}
		}
	}

	hasBaked = true;
}

void TilemapColliderComponent::clearColliders()
{
	GameObject* owner = getGameObject();
	Scene* scene = owner != nullptr ? owner->getScene() : nullptr;
	if ( scene != nullptr )
	{
		for (const ObjectHandle& handle : colliderHandles)
		{
			if ( handle.isValid() )
			{
				scene->removeGameObject(handle);
			}
		}
	}

	colliderHandles.clear();
	hasBaked = false;
}

void TilemapColliderComponent::serialize(WriteArchive& archive) const
{
	archive.process(autoBake);
}

void TilemapColliderComponent::deserialize(ReadArchive& archive)
{
	archive.process(autoBake);
	tilemapComponent = nullptr;
	colliderHandles.clear();
	hasBaked = false;
}

void TilemapColliderComponent::onStart()
{
	if ( autoBake )
	{
		bakeColliders();
	}
}

void TilemapColliderComponent::onDestroy()
{
	clearColliders();
	Behaviour::onDestroy();
}
