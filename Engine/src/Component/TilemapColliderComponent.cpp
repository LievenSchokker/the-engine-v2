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
} // namespace

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

void TilemapColliderComponent::bakeColliders()
{
	clearColliders();

	if ( tilemapComponent == nullptr )
	{
		tilemapComponent = getComponent<TilemapComponent>();
	}

	if ( tilemapComponent == nullptr || !tilemapComponent->isReady() )
	{
		std::cout
			<< "[TilemapColliderComponent] Tilemap not ready, skipping bake."
			<< std::endl;
		return;
	}

	if ( tilemapComponent->getCollidableTileIds().empty() )
	{
		return;
	}

	GameObject* owner = getGameObject();
	if ( owner == nullptr )
	{
		return;
	}

	Scene* scene = owner->getScene();
	if ( scene == nullptr )
	{
		return;
	}

	const Transform* transform = tilemapComponent->getTransform();
	if ( transform == nullptr )
	{
		return;
	}

	const int width = tilemapComponent->getGridWidth();
	const int height = tilemapComponent->getGridHeight();
	if ( width <= 0 || height <= 0 )
	{
		return;
	}

	const Vector2 tileSize = tilemapComponent->getTileSize();
	if ( tileSize.x <= 0.0f || tileSize.y <= 0.0f )
	{
		return;
	}

	const Vector2 origin = transform->getWorldPosition();
	const double rotation = transform->getWorldRotation();
	const Vector2 worldScale =
		Vector2Utils::sanitizeScale(transform->getWorldScale());
	const Vector2 scaledTileSize{
		static_cast<float>(std::abs(tileSize.x * worldScale.x)),
		static_cast<float>(std::abs(tileSize.y * worldScale.y))};

	const size_t cellCount = static_cast<size_t>(width) *
	                         static_cast<size_t>(height);
	std::vector<uint8_t> collidable(cellCount, 0);
	std::vector<uint8_t> visited(cellCount, 0);

	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			Vector2 cell{static_cast<float>(x), static_cast<float>(y)};
			const int tileId = tilemapComponent->getTileAt(cell);
			if ( tilemapComponent->hasTileCollider(tileId) )
			{
				collidable[cellIndex(x, y, width)] = 1;
			}
		}
	}

	int colliderIndex = 0;

	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			const size_t index = cellIndex(x, y, width);
			if ( collidable[index] == 0 || visited[index] == 1 )
			{
				continue;
			}

			int rectWidth = 0;
			while ( (x + rectWidth) < width )
			{
				const size_t rowIndex = cellIndex(x + rectWidth, y, width);
				if ( collidable[rowIndex] == 0 || visited[rowIndex] == 1 )
				{
					break;
				}
				++rectWidth;
			}

			int rectHeight = 1;
			bool canExpand = true;
			while ( (y + rectHeight) < height && canExpand )
			{
				for ( int dx = 0; dx < rectWidth; ++dx )
				{
					const size_t rowIndex =
						cellIndex(x + dx, y + rectHeight, width);
					if ( collidable[rowIndex] == 0 ||
						 visited[rowIndex] == 1 )
					{
						canExpand = false;
						break;
					}
				}

				if ( canExpand )
				{
					++rectHeight;
				}
			}

			for ( int dy = 0; dy < rectHeight; ++dy )
			{
				for ( int dx = 0; dx < rectWidth; ++dx )
				{
					visited[cellIndex(x + dx, y + dy, width)] = 1;
				}
			}

			const Vector2 localSize{tileSize.x * rectWidth,
			                        tileSize.y * rectHeight};
			const Vector2 center{
				origin.x + (static_cast<float>(x) * tileSize.x) +
					(localSize.x * 0.5f),
				origin.y + (static_cast<float>(y) * tileSize.y) +
					(localSize.y * 0.5f)};
			const Vector2 colliderSize{scaledTileSize.x * rectWidth,
			                           scaledTileSize.y * rectHeight};

			auto colliderObject = std::make_unique<GameObject>();
			colliderObject->setName(owner->getName() + "_TileCollider_" +
			                        std::to_string(colliderIndex++));
			colliderObject->setIsStatic(true);

			Transform* colliderTransform = colliderObject->getTransform();
			colliderTransform->setPosition(center);
			colliderTransform->setRotationAngle(rotation);

			auto* collider = colliderObject->addComponent<Collider>();
			collider->setRectangle(colliderSize);

			auto* rigidBody = colliderObject->addComponent<RigidBody>();
			rigidBody->makeStatic();

			ObjectHandle handle = scene->addGameObject(std::move(colliderObject));
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
