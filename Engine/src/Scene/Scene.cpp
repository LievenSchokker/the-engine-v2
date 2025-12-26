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

int Scene::addGameObject(std::unique_ptr<GameObject> gameObject)
{
    if ( gameObject == nullptr ) {
        std::cerr << "[Scene] Error: Attempted to add a null game object\n";
        return -1;
    }

	GameObject* addedObject = gameObject.get();
    gameObjects.emplace_back(std::move(gameObject));
	addedObject->setScene(*this);
    return 0;
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
	gameObjects.erase(it);

	return result;
}

void Scene::onStart(GameWorld& world)
{
	if (active) return;
    active = true;
}

void Scene::onStop()
{
	if ( !active )
	{
		return;
	}

	active = false;
}


void Scene::destroyAllGameObjects()
{
    for (const auto& gameObject : gameObjects )
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


NavigationSystem* Scene::getNavigationSystem()
{
    if (!navigationSystem)
    {
        initialiseNavigationSystem({100, 100, Vector2{15, 15}});
    }
    return navigationSystem.get();
}