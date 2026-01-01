#include "Scene/Scene.h"

#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "AI/Navigation/NavigationObstacle.h"
#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationGridOptions.h"
#include "Scene/Slot.h"

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

    std::vector<Behaviour*> allBehaviours;

    forEachGameObject([&](const GameObject& obj) {
        for (auto& behaviour : obj.getAllBehaviours()) {
            if (behaviour) allBehaviours.emplace_back(behaviour);
        }
    });

    for (const auto& behaviour : beforeEnableBehaviours) {
        behaviour->setEnabled(true);
    }
    beforeEnableBehaviours.clear();
}

void Scene::onStop()
{
    if (!active) return;
    active = false;

    forEachGameObject([&](GameObject& obj) {
        const auto& enabledBehaviours = obj.getEnabledBehaviours();
        beforeEnableBehaviours.insert(
            beforeEnableBehaviours.end(),
            enabledBehaviours.begin(),
            enabledBehaviours.end()
        );
        obj.setBehavioursEnabled(false);
    });
}



///     [[[ GAME OBJECT HANDLING AND LIFETIME FUNCTIONS ]]]


GameObjectHandle Scene::addGameObject(std::unique_ptr<GameObject> gameObject)
{
    if (gameObject == nullptr) {
        std::cerr << "[Scene] Error: Attempted to add a null game object\n";
        return GameObjectHandle::null();
    }

    uint32_t index;

    if (!freeIndices.empty()) {
        index = freeIndices.back();
        freeIndices.pop_back();
    } else {
        index = static_cast<uint32_t>(slots.size());
        slots.push_back({});
    }

    GameObject* obj = gameObject.get();
    slots[index].object = std::move(gameObject);

    GameObjectHandle handle{ index, slots[index].generation };
    obj->setScene(*this);
    return handle;
}



bool Scene::removeGameObject(const std::string& name)
{
    GameObjectHandle handle = findHandleByName(name);
    if (handle.isNull()) return false;

    removeGameObject(handle);
    return true;
}

bool Scene::removeGameObject(GameObjectHandle handle)
{
    if (!isValid(handle)) return false;

    auto& slot = slots[handle.index];

    if (active && slot.object->getIsActive()) {
        slot.object->destroy();
        slot.object->onSceneDestroy();
    }

    slot.object.reset();

    //This generation is super imporant, it makes sure that when an object get's removed and a new object get's
    //assigned the same ID the can differentiate object with same id but different generation handles.
    slot.generation++;
    freeIndices.push_back(handle.index);
    return true;
}



GameObject* Scene::getGameObject(const std::string& name) const
{
    return getGameObject(findHandleByName(name));
}

GameObject* Scene::getGameObject(GameObjectHandle handle) const
{
    if (!isValid(handle)) return nullptr;
    return slots[handle.index].object.get();
}

std::unique_ptr<GameObject> Scene::extractGameObject(const std::string& name)
{
    return extractGameObject(findHandleByName(name));
}


void Scene::destroyAllGameObjects()
{
    for (auto& [object, generation] : slots) {
        if (object) {
            object->onSceneDestroy();
            object.reset();
        }
    }
    slots.clear();
    freeIndices.clear();
}

std::unique_ptr<GameObject> Scene::extractGameObject(GameObjectHandle handle)
{
    if (!isValid(handle)) return nullptr;

    auto& slot = slots[handle.index];

    if (active && slot.object) {
        slot.object->setBehavioursEnabled(false);
    }

    auto result = std::move(slot.object);
    slot.generation++;
    freeIndices.push_back(handle.index);

    return result;
}


//TODO Move this out of scene class
//      [[[ NAVIGATGION SYSTEM FUNCTIONS ]]]


void Scene::initialiseNavigationSystem(NavigationGridOptions options)
{
	std::unique_ptr<NavigationGrid> navGrid = std::make_unique<NavigationGrid>(
		options.gridWidth, options.gridHeight, options.cellSize);
	const std::vector<NavigationObstacle*> obstacles = getAllComponentsOfType<
		NavigationObstacle>();

	std::vector<BoundingBox> obstacleBounds{};

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

GameObjectHandle Scene::findHandleByName(const std::string& name) const
{
    for (uint32_t i = 0; i < slots.size(); i++) {
        if (const auto& [object, generation] = slots[i]; object && object->getName() == name) {
            return GameObjectHandle{ i, generation };
        }
    }
    return GameObjectHandle::null();
}

bool Scene::isValid(const GameObjectHandle handle) const
{
    if (handle.isNull()) return false;
    if (handle.index >= slots.size()) return false;

    const auto& slot = slots[handle.index];
    return slot.generation == handle.generation
        && slot.object != nullptr;
}
