#include "Scene/Scene.h"

#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "Core/ApplicationClock.h"
#include "AI/Navigation/NavigationObstacle.h"
#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationGridOptions.h"
#include "Scene/SceneManager.h"

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


///     [[[ SCENE LIFECYCLE FUNCTIONS ]]]


void Scene::onStart(GameWorld& world)
{
	gameWorld = &world;
	if (active) return;

	initialiseNavigationSystem({100, 100, Vector2{15, 15}});
	active = true;
}


void Scene::onStop()
{
	if (!active) return;
	active = false;
}


///     [[[ GAME OBJECT HANDLING AND LIFETIME FUNCTIONS ]]]

ObjectHandle Scene::addGameObject(std::unique_ptr<GameObject> gameObject)
{
	if (gameObject == nullptr)
	{
		std::cerr << "[Scene] Error: Attempted to add a null game object\n";
		return ObjectHandle::null();
	}

	GameObject* obj = gameObject.get();

	ObjectHandle handle = gameObjects.add(std::move(gameObject));

	obj->setScene(*this);
	obj->setGameObjectHandle(handle);
	if (active && gameWorld && gameWorld->sceneManager)
	{
		std::vector<Behaviour*> behaviours = obj->getAllBehaviours();
		gameWorld->sceneManager->getBehaviourSystem().
				   initialiseRuntimeBehaviours(behaviours, *gameWorld);
	}

	return handle;
}


bool Scene::removeGameObject(const std::string& name)
{
	ObjectHandle handle = findHandleByName(name);
	if (handle.isNull()) return false;
	return removeGameObject(handle);
}

bool Scene::removeGameObject(ObjectHandle handle)
{
	if (!gameObjects.isValid(handle)) return false;

	GameObject* obj = gameObjects.resolve(handle);

	if (active && obj->getIsActive())
	{
		obj->destroy();
		obj->onSceneDestroy();
	}

	gameObjects.destroy(handle);
	return true;
}

GameObject* Scene::getGameObject(const std::string& name)
{
	return getGameObject(findHandleByName(name));
}

GameObject* Scene::getGameObject(ObjectHandle handle)
{
	return gameObjects.resolve(handle);
}

const GameObject* Scene::getGameObject(const std::string& name) const
{
	return getGameObject(findHandleByName(name));
}

const GameObject* Scene::getGameObject(ObjectHandle handle) const
{
	return gameObjects.resolve(handle);
}

std::unique_ptr<GameObject> Scene::extractGameObject(const std::string& name)
{
	return extractGameObject(findHandleByName(name));
}

std::unique_ptr<GameObject> Scene::extractGameObject(ObjectHandle handle)
{
	if (!gameObjects.isValid(handle)) return nullptr;
	return gameObjects.extract(handle);
}

void Scene::destroyAllGameObjects()
{
	forEachGameObject([](GameObject& obj)
	{
		obj.onSceneDestroy();
	});
	gameObjects.clear();
}

//TODO Move this out of scene class
//      [[[ NAVIGATGION SYSTEM FUNCTIONS ]]]


void Scene::initialiseNavigationSystem(NavigationGridOptions options)
{
	auto navGrid = std::make_unique<NavigationGrid>(
		options.gridWidth, options.gridHeight, options.cellSize);
	const std::vector<NavigationObstacle*> obstacles = getAllComponentsOfType<
		NavigationObstacle>();

	std::vector<BoundingBox> obstacleBounds{};

	obstacleBounds.reserve(obstacles.size());

	for (const auto& obstacle : obstacles)
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


///      [[[ HELPER FUNCTIONS ]]]


ObjectHandle Scene::findHandleByName(const std::string& name) const
{
	const auto& slots = gameObjects.getSlots();

	for (uint32_t i = 0; i < slots.size(); i++)
	{
		const auto& [object, generation] = slots[i];
		if (object && object->getName() == name)
		{
			return ObjectHandle{i, generation};
		}
	}
	return ObjectHandle::null();
}

bool Scene::isValid(const ObjectHandle handle) const
{
	return gameObjects.isValid(handle);
}

void Scene::serialize(WriteArchive& archive) const
{
    std::string sceneName = name;
    archive.process(sceneName);

    std::map<const GameObject*, uint32_t> goToIndex;
    uint32_t goCount = 0;

    const auto& slots = gameObjects.getSlots();
    for (uint32_t i = 0; i < slots.size(); ++i)
    {
        const auto& [object, generation] = slots[i];
        if (object)
        {
            goToIndex[object.get()] = goCount;
            ++goCount;
        }
    }

    archive.process(goCount);

    for (const auto& [object, generation] : slots)
    {
        if (object)
        {
            object->serialize(archive);
        }
    }

    for (const auto& [object, generation] : slots)
    {
        if (object)
        {
            int32_t parentIndex = -1;
            if (object->getParent())
            {
                auto it = goToIndex.find(object->getParent());
                if (it != goToIndex.end())
                {
                    parentIndex = static_cast<int32_t>(it->second);
                }
            }
            archive.process(parentIndex);
        }
    }
}

void Scene::deserialize(ReadArchive& archive)
{
    destroyAllGameObjects();
    archive.process(name);

    uint32_t goCount;
    archive.process(goCount);

    std::vector<GameObject*> loadedObjects;
    loadedObjects.reserve(goCount);

    for (uint32_t i = 0; i < goCount; ++i)
    {
        auto go = std::make_unique<GameObject>();
        go->deserialize(archive);

        GameObject* rawPtr = go.get();
        addGameObject(std::move(go));
        loadedObjects.push_back(rawPtr);
    }

    for (uint32_t i = 0; i < goCount; ++i)
    {
        int32_t parentIndex;
        archive.process(parentIndex);

        if (parentIndex >= 0 && parentIndex < static_cast<int32_t>(loadedObjects.size()))
        {
            loadedObjects[i]->setParent(loadedObjects[static_cast<size_t>(parentIndex)]);
        }
    }
}
