#include "../../inc/Scene/Scene.h"

#include "../../inc/Component/ComponentManager.h"
#include "../../inc/Component/ShapeRenderer.h"
#include "../../inc/Rendering/RenderQueue.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "Behaviour/Behaviour.h"

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

    std::vector<Behaviour*> allBehaviours;

    /// Retrieve every behaviour on every GameObject in this scene object.
	for ( auto& gameObject : gameObjects )
	{
        for (auto& behaviour : gameObject->getAllBehaviours())
        {
            if (behaviour == nullptr)
                continue;

            /// Store in local vector to iterate over more easily.
            allBehaviours.emplace_back(behaviour);

            /// Awake should only be called once per behaviour.
            if (!behaviour->getHasAwakened())
                behaviour->awake();
        }
	}

    /// Use local vector to iterate over all behaviours in this scene object.
    for ( auto& behaviour : allBehaviours )
    {
        /// Call onEnable only on enabled behaviours, and AFTER awake has been called on EVERY other behaviour
        if (behaviour->getIsActiveAndEnabled())
        {
            behaviour->onEnable();
        }
    }

    /// Call start on all behaviours, AFTER Awake and OnEnable have both been called on ALL other behaviours.
    for ( auto& behaviour : allBehaviours )
    {
        /// Start should only be called if the gameobject of the behaviour is active, and the behaviour itself is enabled
        if (behaviour->getIsActiveAndEnabled())
        {
            /// Start should only be called once per behaviour.
            if (!behaviour->getHasStarted())
                behaviour->start();
        }
    }

}

void Scene::onStop()
{
	if ( !active ) {
		return;
	}

	active = false;
	for ( auto& gameObject : gameObjects )
	{
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

    /// Update all GameObject's behaviours:
    for ( auto& gameObject : gameObjects )
    {
        for (const auto& behaviour : gameObject->getEnabledBehaviours())
        {
            behaviour->update();
        }
    }
}

void Scene::collectRenderCommands(std::vector<ShapeRenderCommand>& out) const
{
	if ( !active ) {
		return;
	}

	for ( const auto& gameObject : gameObjects ) {
		if ( !gameObject->getIsActive() ) {
			continue;
		}

		auto* shapeRenderer = gameObject->getComponent<ShapeRenderer>();
		if ( shapeRenderer != nullptr ) {
			const auto command = shapeRenderer->buildRenderCommand();
			if ( command.has_value() ) {
				out.emplace_back(*command);
			}
		}
	}
}
