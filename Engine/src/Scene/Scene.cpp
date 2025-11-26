#include "Scene/Scene.h"

#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "Component/ComponentManager.h"
#include "Component/ShapeRenderer.h"
#include "Rendering/RenderQueue.h"

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

    gameObject->setScene(*this);
	gameObjects.emplace_back(std::move(gameObject));

    if (active)
	{
	    /// Call awake, onEnable and start methods on each behaviour of the added GO:
	    initialiseBehaviours(gameObject->getAllBehaviours());
	}

	return true;
}

bool Scene::removeGameObject(const std::string& name)
{
	const auto it =
		std::remove_if(gameObjects.begin(), gameObjects.end(),
					   [&](const std::unique_ptr<GameObject>& gameObject) {
						   if ( gameObject->getName() == name ) {
							   if ( active && gameObject->getIsActive() )
							       {
								   // TODO: call gameobject on stop
							       /// GO does not have (and shouldn't have) an onStop, but we can destroy the object:
							       gameObject->destroy();
							       gameObject->onSceneDestroy();
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
	if ( active && it->get()->getIsActive())
	{
		// TODO: call gameobject on stop
	    /// GO does not have (and shouldn't have) an onStop, but we can deactivate all behaviours:
	    it->get()->setBehavioursEnabled(false);
	}

	// Move ownership and remove from vector
	auto result = std::move(*it);
	gameObjects.erase(it);
	return result;
}

void Scene::onStart()
{
    std::cout << "Scene::onStart called for " << name << std::endl;
	if ( active ) {
		return;
	}

	active = true;

    /// Store all behaviours in this scene object:
    std::vector<Behaviour*> allBehaviours;

    /// Retrieve every behaviour on every GameObject in this scene object.
	for ( auto& gameObject : gameObjects )
	{
        for (auto& behaviour : gameObject->getAllBehaviours())
        {
            if (behaviour == nullptr)
                continue;

            allBehaviours.emplace_back(behaviour);
        }
	}

    /// Initialise all the behaviours by calling their lifetime functions in the correct order.
    initialiseBehaviours(allBehaviours);
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
	    /// GO does not have (and shouldn't have) an onStop, but we can deactivate all behaviours:
        gameObject->setBehavioursEnabled(false);
	}
}

void Scene::onPause()
{
	if ( !active ) {
		return;
	}

	for ( auto& gameObject : gameObjects ) {
		// TODO: call gameobject on pause
	    /// GO does not have (and shouldn't have) an onPause, but we can deactivate all behaviours:
	    gameObject->setBehavioursEnabled(false);
	}
}

void Scene::onResume()
{
	if ( !active ) {
		return;
	}

	for ( auto& gameObject : gameObjects ) {
		// TODO: call gameobject on resume
	    /// GO does not have (and shouldn't have) an onResume, but we can reactivate all behaviours:
	    gameObject->setBehavioursEnabled(true);

	}
}

void Scene::update(float deltaTime)
{
	if ( !active ) {
		return;
	}

    /// Update all GameObject's behaviours:
    for ( auto& gameObject : gameObjects )
    {
        /// Only behaviours on active GameObjects should be updated.
        if (!gameObject->getIsActive())
            continue;

        /// Iterate over all enabled behaviours.
        for (const auto& behaviour : gameObject->getEnabledBehaviours())
        {
            /// Only update
            if (!behaviour->getHasAwakened() || !behaviour->getHasStarted())
                continue;
            behaviour->update();
        }
    }

    processDestroyQueue();
}

void Scene::collectRenderCommands(std::vector<ShapeRenderCommand>& out) const
{
    if (!active) return;
    for (const auto& gameObject : gameObjects)
    {
        if (!gameObject->getIsActive()) continue;

        auto* shapeRenderer = gameObject->getComponent<ShapeRenderer>();
        if (shapeRenderer)
        {
            auto cmd = shapeRenderer->buildRenderCommand();
            if (cmd.has_value())
            {
                out.push_back(cmd.value());
            }
        }
    }
}

void Scene::initialiseBehaviours(const std::vector<Behaviour *> &behaviours)
{
    for (auto& behaviour : behaviours)
    {
        if (behaviour == nullptr)
            continue;

        if (!behaviour->getHasAwakened())
            behaviour->awake();
    }

    for (auto& behaviour : behaviours)
    {
        if (behaviour == nullptr)
            continue;

        if (behaviour->getIsActiveAndEnabled())
            behaviour->onEnable();
    }

    for (auto& behaviour : behaviours)
    {
        if (behaviour == nullptr)
            continue;

        if (!behaviour->getIsActiveAndEnabled())
            continue;

        if (!behaviour->getHasStarted())
            behaviour->start();
    }
}

void Scene::queueDestroy(GameObject *obj)
{
    for (auto* queued : destroyQueue)
    {
        if (queued == obj)
            return;
    }

    destroyQueue.push_back(obj);
}


void Scene::processDestroyQueue()
{
    for (GameObject* gameObject : destroyQueue)
    {

        gameObject->onSceneDestroy();


        gameObjects.erase(
            std::remove_if(
                gameObjects.begin(),
                gameObjects.end(),
                [gameObject](const std::unique_ptr<GameObject>& ptr) {
                    return ptr.get() == gameObject;
                }
            ),

            gameObjects.end()
        );
    }

    destroyQueue.clear();
}

void Scene::setWorld(GameWorld* world)
{
    gameWorld = world;
}
