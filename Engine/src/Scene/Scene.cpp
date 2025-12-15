#include "Scene/Scene.h"

#include "Behaviour/Behaviour.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "Component/BaseComponentTypes/UIRenderComponent.h"
#include "Component/ComponentManager.h"
#include "GameObject/GameObject.h"
#include "Rendering/RenderQueue/RenderQueue.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "Game.h"
#include "Behaviour/Behaviour.h"

Scene::Scene(std::string name) : name(std::move(name))
{
}

Scene::~Scene()
{
	destroyAllGameObjects();
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
    GameObject* addedObject = gameObjects.back().get();
    addedObject->setScene(*this);

    return true;
}

bool Scene::addRunTimeGameObject(std::unique_ptr<GameObject> gameObject, GameWorld& world)
{
	if (gameObject == nullptr) {
		std::cerr << "[Scene] Error: Attempted to add a null game object\n";
		return false;
	}

	GameObject* addedObject = gameObject.get();
	addGameObjectInternal(std::move(gameObject));
	addedObject->setScene(*this);

	if (active) {
		initialiseBehaviours(addedObject->getAllBehaviours(), world);
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

	if ( it != gameObjects.end() )
	{
        GameObject* gameObject = it->get();
	    removeGameObjectInternal(gameObject);
		return true;
	}

	return false;
}

GameObject* Scene::getGameObject(const std::string& name) const
{
	for ( const auto& gameObject : gameObjects )
	{
		if ( gameObject->getName() == name )
		{
			return gameObject.get();
		}
	}

	return nullptr;
}

std::unique_ptr<GameObject> Scene::extractGameObject(const std::string& name)
{
	auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
						   [&](const std::unique_ptr<GameObject>& gameObject)
						   { return gameObject->getName() == name; });

	if ( it == gameObjects.end() )
	{
		return nullptr;
	}

    GameObject* gameObject = it->get();

	// Call onStop if scene is active
	if (active && gameObject != nullptr)
	{
	    gameObject->setBehavioursEnabled(false);
	}

	// Move ownership and remove from vector
	auto result = std::move(*it);
    removeGameObjectInternal(gameObject);

	return result;
}

void Scene::onStart(GameWorld& world)
{
	if (active) return;

	active = true;

	std::vector<Behaviour*> allBehaviours;
	for (auto& gameObject : gameObjects) {
		for (auto& behaviour : gameObject->getAllBehaviours()) {
			if (behaviour != nullptr)
				allBehaviours.emplace_back(behaviour);
		}
	}

	initialiseBehaviours(allBehaviours, world);
}

void Scene::onStop()
{
	if ( !active )
	{
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
	if ( !active )
	{
		return;
	}

	for ( auto& gameObject : gameObjects )
	{
	    gameObject->setBehavioursEnabled(false);
	}
}

void Scene::onResume()
{
	if ( !active )
	{
		return;
	}

	for ( auto& gameObject : gameObjects )
	{
	    gameObject->setBehavioursEnabled(true);

	}
}

void Scene::update(double deltaTime, GameWorld* world)
{
	if ( !active )
	{
		return;
	}

	for ( auto& gameObject : gameObjects )
	{
		if (!gameObject->getIsActive())
			continue;

		for ( const auto& behaviour : gameObject->getEnabledBehaviours() )
		{
			if ( !behaviour->getHasAwakened() || !behaviour->getHasStarted() )
				continue;
			behaviour->update(deltaTime, world);
		}

	}

	processDestroyQueue();
}

void Scene::initialiseBehaviours(const std::vector<Behaviour *> &behaviours, GameWorld& world)
{
	/// First call awake on all behaviours:
	for ( auto& behaviour : behaviours )
	{
		if ( behaviour == nullptr )
		    continue;

        /// Awake may only be called once per behaviour
        if (!behaviour->getHasAwakened())
            behaviour->awake(world);
    }

    /// Then call onEnable on all enabled behaviours on active GameObjects:
    for (auto& behaviour : behaviours)
    {
        if (behaviour == nullptr)
            continue;

        if (behaviour->getIsActiveAndEnabled())
            behaviour->onEnable();
    }

    /// Lastly call start on all enabled behaviours on Active GameObjects:
    for (auto& behaviour : behaviours)
    {
        if (behaviour == nullptr)
            continue;

        if (!behaviour->getIsActiveAndEnabled())
            continue;

        /// Start may only be called once per behaviour
        if (!behaviour->getHasStarted())
            behaviour->start();
    }
}

void Scene::queueDestroy(GameObject* obj)
{
	/// Check if the object is already in the destroyQueue.
	for ( auto* queued : destroyQueue )
	{
		if ( queued == obj ) return;
	}

	destroyQueue.push_back(obj);
}

void Scene::processDestroyQueue()
{
    for (GameObject* gameObject : destroyQueue)
    {
        gameObject->onSceneDestroy();
        removeGameObjectInternal(gameObject);
    }

	/// Clear the queue when all queued objects have been deleted.
	destroyQueue.clear();
}

bool Scene::isInDestroyQueue(GameObject* obj)
{
    if (obj == nullptr)
        return false;

    return std::find(destroyQueue.begin(), destroyQueue.end(), obj)
            != destroyQueue.end();
}


void Scene::destroyAllGameObjects()
{
    for ( auto& gameObject : gameObjects )
    {
        gameObject->onSceneDestroy();
    }

	gameObjects.clear();
}


int Scene::getSceneId(const GameObject& gameObject) const
{
    auto it = gameObjectIds.find(&gameObject);

    if (it == gameObjectIds.end())
        return -1;

    return it->second;
}


GameObject* Scene::getGameObjectById(int id) const
{
    for (const auto& obj : gameObjects)
    {
        if (getSceneId(*obj) == id)
            return obj.get();
    }

    return nullptr;
}


bool Scene::removeGameObjectInternal(GameObject* gameObject)
{
    if (gameObject == nullptr)
        return false;

    auto it = std::find_if(
        gameObjects.begin(),
        gameObjects.end(),
        [gameObject](const std::unique_ptr<GameObject>& ptr) {
            return ptr.get() == gameObject;
        }
    );

    if (it == gameObjects.end())
        return false;

    gameObjectIds.erase(gameObject);
    gameObjects.erase(it);

    return true;
}

bool Scene::addGameObjectInternal(std::unique_ptr<GameObject> gameObject)
{
    if (!gameObject)
        return false;

    GameObject* goRaw = gameObject.get();

    gameObjects.emplace_back(std::move(gameObject));

    gameObjectIds[goRaw] = currentGameObjectId;
    currentGameObjectId++;

    return true;
}
