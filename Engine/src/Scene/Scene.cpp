#include "Scene/Scene.h"

#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "Component/BaseComponentTypes/UIRenderComponent.h"
#include "Core/ApplicationClock.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "AI/Navigation/NavigationObstacle.h"
#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationSystem.h"


#include <algorithm>
#include <iostream>
#include <utility>

#include "Game.h"
#include "Behaviour/Behaviour.h"

#include "AI/Navigation/NavigationGridOptions.h"

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

	GameObject* addedObject = gameObject.get();
	addGameObjectInternal(std::move(gameObject));
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
		std::ranges::remove_if(gameObjects,
							   [&](const std::unique_ptr<GameObject>& gameObject)
							   {
								   if ( gameObject->getName() == name )
								   {
									   if ( active && gameObject->getIsActive() )
									   {
										   gameObject->destroy();
										   gameObject->onSceneDestroy();
									   }
									   return true;
								   }
								   return false;
							   }).begin();

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
	const auto it = std::ranges::find_if(gameObjects,
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
	gameObjectIds.erase(gameObject);
	gameObjects.erase(it);

	return result;
}

void Scene::onStart(GameWorld& world)
{
	if (active) return;
    active = true;
    initialiseNavigationSystem({100, 100, Vector2{15, 15}});
}

void Scene::onStop()
{
	if ( !active )
	{
		return;
	}

	active = false;
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

void Scene::update(double deltaTime, const GameWorld& world)
{
	if ( !active )
	{
		return;
	}

	bool clockPaused = (world.clock != nullptr &&
						world.clock->isPaused());

    std::vector<GameObject*> objectsToUpdate;
    objectsToUpdate.reserve(gameObjects.size());

    for (auto& gameObject : gameObjects)
    {
        if (gameObject && gameObject->getIsActive())
        {
            objectsToUpdate.push_back(gameObject.get());
        }
    }

    for (GameObject* gameObject : objectsToUpdate)
    {
        if (!gameObject || isInDestroyQueue(gameObject))
        {
            continue;
        }

        if (!gameObjectIds.contains(gameObject))
        {
            continue;
        }

        for (Behaviour* behaviour : gameObject->getEnabledBehaviours())
        {
            if (!behaviour || !behaviour->getHasAwakened() || !behaviour->getHasStarted())
            {
                continue;
            }

            if (clockPaused && !behaviour->shouldRunWhenPaused())
            {
                continue;
            }

            behaviour->update(deltaTime, world);
        }
    }

	processDestroyQueue();
}

void Scene::initialiseBehaviours(const std::vector<Behaviour *> &behaviours, GameWorld& world)
{
    for ( auto& behaviour : behaviours )
    {
        if ( behaviour == nullptr )
            continue;
        if (!behaviour->getHasAwakened())
            behaviour->awake(world);
    }

    /// Then call onEnable on all enabled behaviours on active GameObjects:
    for (auto& behaviour : behaviours)
    {
        if (behaviour == nullptr)
            continue;

        if (behaviour->getGameObject()->getIsActive() && behaviour->getIsEnabled())
        {
            behaviour->onEnable();
        }
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

std::unique_ptr<GameObject> Scene::extractGameObject(GameObject* obj)
{
    for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it)
    {
        if (it->get() == obj)
        {
            std::unique_ptr<GameObject> extracted = std::move(*it);
            gameObjects.erase(it);
            return extracted;
        }
    }
    return nullptr;
}

void Scene::removeGameObject(GameObject* obj)
{
    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(),
            [obj](const std::unique_ptr<GameObject>& ptr) {
                return ptr.get() == obj;
            }),
        gameObjects.end()
    );
}

std::vector<std::unique_ptr<GameObject>>& Scene::getGameObjects()
{
    return gameObjects;
}

const std::vector<std::unique_ptr<GameObject>>& Scene::getGameObjects() const
{
    return gameObjects;
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

    auto it = std::ranges::find_if(gameObjects
                                   ,
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

 void Scene::initialiseNavigationSystem(NavigationGridOptions options)
{
    std::unique_ptr<NavigationGrid> navGrid = std::make_unique<NavigationGrid>(options.gridWidth, options.gridHeight, options.cellSize);
    std::vector<NavigationObstacle*> obstacles = getAllComponentsOfType<NavigationObstacle>();

    std::vector<BoundingBox> obstacleBounds {};

    for ( const auto& obstacle : obstacles )
    {
        obstacleBounds.push_back(obstacle->getBounds());
    }

    navigationSystem = std::make_unique<NavigationSystem>(std::move(navGrid));
    navigationSystem->bake(obstacleBounds);
}


NavigationSystem* Scene::getNavigationSystem() const
{
    return navigationSystem.get();
}