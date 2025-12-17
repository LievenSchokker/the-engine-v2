#include "Networking/Component/ComponentRegistry.h"
#include "Component/Transform.h"
#include "Component/ShapeRenderer.h"
#include "Component/GridComponent.h"
#include "Component/TilemapComponent.h"
#include "Demo/PlayerMovement.h"
#include "Networking/Component/ComponentFactory.h"

void registerSerializableComponents()
{
	auto& factory = ComponentFactory::instance();

	factory.registerComponent(ComponentType::Transform, []() {
		return std::make_unique<Transform>();
	});

	factory.registerComponent(ComponentType::ShapeRenderer, []() {
		return std::make_unique<ShapeRenderer>();
	});

	factory.registerComponent(ComponentType::Grid, []() {
		return std::make_unique<GridComponent>();
	});

	factory.registerComponent(ComponentType::TileMap, []() {
		return std::make_unique<TilemapComponent>();
	});

	factory.registerComponent(ComponentType::NetworkIdentity, []() {
		return std::make_unique<NetworkIdentity>();
	});

	factory.registerComponent(ComponentType::PlayerMovement, []() {
		return std::make_unique<PlayerMovement>();
	});
}