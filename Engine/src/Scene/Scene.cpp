#include "Scene/Scene.h"

#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include "AI/Navigation/NavigationObstacle.h"
#include "AI/Navigation/NavigationGrid.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationGridOptions.h"
#include "../../inc/Scene/SlotMap/Slot.h"

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


void Scene::onStart(GameWorld& world) {
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


void Scene::onStop() {
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

ObjectHandle Scene::addGameObject(std::unique_ptr<GameObject> gameObject) {
	if (gameObject == nullptr) {
		std::cerr << "[Scene] Error: Attempted to add a null game object\n";
		return ObjectHandle::null();
	}

	GameObject* obj = gameObject.get();
	ObjectHandle handle = gameObjects.add(std::move(gameObject));

	obj->setScene(*this);
	obj->setGameObjectHandle(handle);

	return handle;
}

bool Scene::removeGameObject(const std::string& name) {
	ObjectHandle handle = findHandleByName(name);
	if (handle.isNull()) return false;
	return removeGameObject(handle);
}

bool Scene::removeGameObject(ObjectHandle handle) {
	if (!gameObjects.isValid(handle)) return false;

	GameObject* obj = gameObjects.resolve(handle);

	if (active && obj->getIsActive()) {
		obj->destroy();
		obj->onSceneDestroy();
	}

	gameObjects.destroy(handle);
	return true;
}

const GameObject* Scene::getGameObject(const std::string& name) const {
	return getGameObject(findHandleByName(name));
}

const GameObject* Scene::getGameObject(ObjectHandle handle) const {
	return gameObjects.resolve(handle);
}

std::unique_ptr<GameObject> Scene::extractGameObject(const std::string& name) {
	return extractGameObject(findHandleByName(name));
}

std::unique_ptr<GameObject> Scene::extractGameObject(ObjectHandle handle) {
	if (!gameObjects.isValid(handle)) return nullptr;

	GameObject* obj = gameObjects.resolve(handle);

	if (active && obj) {
		obj->setBehavioursEnabled(false);
	}

	return gameObjects.extract(handle);
}

void Scene::destroyAllGameObjects() {
	forEachGameObject([](GameObject& obj) {
		obj.onSceneDestroy();
	});
	gameObjects.clear();
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


ObjectHandle Scene::findHandleByName(const std::string& name) const {
	const auto& slots = gameObjects.getSlots();

	for (uint32_t i = 0; i < slots.size(); i++) {
		const auto& slot = slots[i];
		if (slot.object && slot.object->getName() == name) {
			return ObjectHandle{i, slot.generation};
		}
	}
	return ObjectHandle::null();
}

bool Scene::isValid(ObjectHandle handle) const {
	return gameObjects.isValid(handle);
}