#include "../../inc/Scene/Scene.h"

#include "../../inc/Component/ComponentManager.h"
#include "../../inc/Component/GridComponent.h"
#include "../../inc/Component/ShapeRenderer.h"
#include "../../inc/Component/TilemapComponent.h"
#include "../../inc/Rendering/RenderQueue.h"

#include <algorithm>
#include <iostream>
#include <utility>

Scene::Scene(std::string name) : name(std::move(name))
{
}

const std::string& Scene::getName() const
{
	return name;
}

bool Scene::addGameObject(std::unique_ptr<GameObject> gameObject)
{
	if ( gameObject == nullptr ) {
		std::cerr << "[Scene] Error: Attempted to add a null game object\n";
		return false;
	}

	bool isGameObjectActive = gameObject->getIsActive();
	gameObjects.emplace_back(std::move(gameObject));

	if ( active && isGameObjectActive ) {
		// TODO: call gameobject on start
	}

	return true;
}

bool Scene::removeGameObject(const std::string& name)
{
	const auto it =
		std::remove_if(gameObjects.begin(), gameObjects.end(),
					   [&](const std::unique_ptr<GameObject>& gameObject) {
						   if ( gameObject->getName() == name ) {
							   if ( active && gameObject->getIsActive() ) {
								   // TODO: call gameobject on stop
							   }
							   return true;
						   }
						   return false;
					   });

	if ( it != gameObjects.end() ) {
		gameObjects.erase(it, gameObjects.end());
		return true;
	}

	return false;
}

GameObject* Scene::getGameObject(const std::string& name) const
{
	for ( const auto& gameObject : gameObjects ) {
		if ( gameObject->getName() == name ) {
			return gameObject.get();
		}
	}

	return nullptr;
}

std::unique_ptr<GameObject> Scene::extractGameObject(const std::string& name)
{
	auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
						   [&](const std::unique_ptr<GameObject>& gameObject) {
							   return gameObject->getName() == name;
						   });

	if ( it == gameObjects.end() ) {
		return nullptr;
	}

	// Call onStop if scene is active
	if ( active && it->get()->getIsActive() ) {
		// TODO: call gameobject on stop
	}

	// Move ownership and remove from vector
	auto result = std::move(*it);
	gameObjects.erase(it);
	return result;
}

void Scene::onStart()
{
	if ( active ) {
		return;
	}

	active = true;
	for ( auto& gameObject : gameObjects ) {
		// TODO: call gameobject on start
	}
}

void Scene::onStop()
{
	if ( !active ) {
		return;
	}

	active = false;
	for ( auto& gameObject : gameObjects ) {
		// TODO: call gameobject on stop
	}
}

void Scene::onPause()
{
	if ( !active ) {
		return;
	}

	for ( auto& gameObject : gameObjects ) {
		// TODO: call gameobject on pause
	}
}

void Scene::onResume()
{
	if ( !active ) {
		return;
	}

	for ( auto& gameObject : gameObjects ) {
		// TODO: call gameobject on resume
	}
}

void Scene::update(float deltaTime) const
{
	if ( !active ) {
		return;
	}

	// TODO: call gameobject update
}

void Scene::collectRenderCommands(std::vector<ShapeRenderCommand>& out) const
{
	if ( !active ) {
		return;
	}

	std::vector<ShapeRenderCommand> debugCommands;
	for ( const auto& gameObject : gameObjects ) {
		if ( !gameObject->getIsActive() ) {
			continue;
		}

		// Collect ShapeRenderer commands
		auto* shapeRenderer = gameObject->getComponent<ShapeRenderer>();
		if ( shapeRenderer != nullptr ) {
			const auto command = shapeRenderer->buildRenderCommand();
			if ( command.has_value() ) {
				out.emplace_back(*command);
			}
		}

		// Collect TilemapComponent commands
		auto* tilemapComponent = gameObject->getComponent<TilemapComponent>();
		if ( tilemapComponent != nullptr ) {
			const auto tilemapCommands =
				tilemapComponent->buildRenderCommands();
			out.insert(out.end(), tilemapCommands.begin(),
					   tilemapCommands.end());
		}

		// Collect GridComponent debug render commands
		auto* gridComponent = gameObject->getComponent<GridComponent>();
		if ( gridComponent != nullptr &&
			 gridComponent->isDebugRenderEnabled() ) {
			const auto gridCommands = gridComponent->buildDebugRenderCommands();
			debugCommands.insert(debugCommands.end(), gridCommands.begin(),
								 gridCommands.end());
		}
	}

	// Append deferred debug overlays after regular render commands.
	out.insert(out.end(), debugCommands.begin(), debugCommands.end());
}
