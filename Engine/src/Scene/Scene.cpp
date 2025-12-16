#include "Scene/Scene.h"

#include "Behaviour/Behaviour.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "Component/BaseComponentTypes/UIRenderComponent.h"
#include "Component/ComponentManager.h"
#include "Core/ApplicationClock.h"
#include "GameObject/GameObject.h"
#include "Rendering/RenderQueue/RenderQueue.h"

#include <algorithm>
#include <iostream>
#include <utility>

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
	if ( gameObject == nullptr )
	{
		std::cerr << "[Scene] Error: Attempted to add a null game object\n";
		return false;
	}

    GameObject* addedObject = gameObject.get();
    addGameObjectInternal(std::move(gameObject));
    addedObject->setScene(*this);

	if ( active )
	{
	    /// Call awake, onEnable and start methods on each behaviour of the added GO:
	    initialiseBehaviours(addedObject->getAllBehaviours());
	}

	return true;
}

bool Scene::removeGameObject(const std::string& name)
{
	const auto it =
		std::remove_if(gameObjects.begin(), gameObjects.end(),
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
	gameObjectIds.erase(gameObject);
	gameObjects.erase(it);
    removeGameObjectInternal(gameObject);

	return result;
}

void Scene::onStart()
{
	if ( active )
	{
		return;
	}

	active = true;

	/// Store all behaviours in this scene object:
	std::vector<Behaviour*> allBehaviours;

	/// Retrieve every behaviour on every GameObject in this scene object.
	for ( auto& gameObject : gameObjects )
	{
		for ( auto& behaviour : gameObject->getAllBehaviours() )
		{
			if ( behaviour == nullptr ) continue;

			allBehaviours.emplace_back(behaviour);
		}
	}

	/// Initialise all the behaviours by calling their lifetime functions in the
	/// correct order.
	initialiseBehaviours(allBehaviours);
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

void Scene::update(double deltaTime, const GameWorld& world)
{
	if ( !active )
	{
		return;
	}

	int behaviorCount = 0;
	bool clockPaused = (world.clock != nullptr &&
						world.clock->isPaused());
	for ( auto& gameObject : gameObjects )
	{
		if (!gameObject->getIsActive())
			continue;

		for ( const auto& behaviour : gameObject->getEnabledBehaviours() )
		{
			if ( !behaviour->getHasAwakened() || !behaviour->getHasStarted() )
				continue;
			// Skip simulation behaviors when paused, but allow behaviors that
			// override shouldRunWhenPaused() to return true (e.g., debug
			// controls)
			if ( clockPaused && !behaviour->shouldRunWhenPaused() )
			{
				continue;  // Skip simulation behaviors when paused
			}
			behaviorCount++;
			behaviour->update(deltaTime, world);
		}
	}

	processDestroyQueue();
}

//
// void Scene::collectRenderCommands(std::vector<ShapeRenderCommand>& out) const
// {
// 	if ( !active ) {
// 		return;
// 	}
//
// 	std::vector<ShapeRenderCommand> debugCommands;
// 	for ( const auto& gameObject : gameObjects ) {
// 		if ( !gameObject->getIsActive() ) {
// 			continue;
// 		}
//
// 		// Collect ShapeRenderer commands
// 		auto* shapeRenderer = gameObject->getComponent<ShapeRenderer>();
// 		if ( shapeRenderer != nullptr ) {
// 			const auto command = shapeRenderer->buildRenderCommand();
// 			if ( command.has_value() ) {
// 				out.emplace_back(*command);
// 			}
// 		}
//
// 		// Collect TilemapComponent commands
// 		auto* tilemapComponent = gameObject->getComponent<TilemapComponent>();
// 		if ( tilemapComponent != nullptr ) {
// 			const auto tilemapCommands =
// 				tilemapComponent->buildRenderCommands();
// 			out.insert(out.end(), tilemapCommands.begin(),
// 					   tilemapCommands.end());
// 		}
//
// 		// Collect GridComponent debug render commands
// 		auto* gridComponent = gameObject->getComponent<GridComponent>();
// 		if ( gridComponent != nullptr &&
// 			 gridComponent->isDebugRenderEnabled() ) {
// 			const auto gridCommands = gridComponent->buildDebugRenderCommands();
// 			debugCommands.insert(debugCommands.end(), gridCommands.begin(),
// 								 gridCommands.end());
// 		}
// 	}
//
// 	// Append deferred debug overlays after regular render commands.
// 	out.insert(out.end(), debugCommands.begin(), debugCommands.end());
// }

void Scene::initialiseBehaviours(const std::vector<Behaviour*>& behaviours)
{
	/// First call awake on all behaviours:
	for ( auto& behaviour : behaviours )
	{
		if ( behaviour == nullptr )
		    continue;

        /// Awake may only be called once per behaviour
        if (!behaviour->getHasAwakened())
            behaviour->awake();
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
//
// void Scene::collectRenderCommands(RenderQueue& queue) const
// {
// 	if ( !active )
// 	{
// 		return;
// 	}
//
// 	// Collect commands from RenderComponents
// 	for ( auto* component : getAllComponentsOfType<RenderComponent>() )
// 	{
// 		if ( component == nullptr ) continue;
// 		component->fillRenderQueue(queue);
// 	}
//
// 	// Collect commands from UserInterfaceRenderComponents
// 	for ( auto* component :
// 		  getAllComponentsOfType<UserInterfaceRenderComponent>() )
// 	{
// 		if ( component == nullptr ) continue;
// 		component->fillUserInterfaceRenderQueue(queue);
// 	}
// }

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
