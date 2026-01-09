#include "Scene/SceneManager.h"

#include "GameObject/GameObject.h"
#include "Component/Transform.h"
#include "Component/NetworkIdentity.h"
#include "Behaviour/NetworkBehaviour.h"
#include "Networking/NetworkSpawnManager.h"
#include "Physics/Components/RigidBody.h"
#include "Scene/SubSystems/NetworkSystem.h"
#include "Physics/IPhysicsWorld.h"
#include "Math/Vector2.h"

#include <iostream>
#include <utility>

#include "Assets/SpritesheetLoader.h"
#include "Component/SpriteComponent.h"

SceneManager::SceneManager(GameWorld &gameWorld)
	: gameWorld(&gameWorld),
	scenes(std::unordered_map<std::string, std::unique_ptr<Scene> >()),
	behaviourSystem(std::make_unique<BehaviourSystem>()),
	destroySystem(std::make_unique<DestroySystem>()),
	networkSystem(std::make_unique<NetworkSystem>())
{
}


SystemStatus SceneManager::start(GameWorld &gameWorld)
{
	this->gameWorld = &gameWorld;
	gameWorld.sceneManager = this;
	return SystemStatus::RUNNING;
}

void SceneManager::fixedUpdate(double deltaTime, const GameWorld &gameWorld)
{
	if (!activeScene || paused) return;

	behaviourSystem->update(*activeScene, deltaTime, gameWorld, true);

	if (persistentScene)
	{
		//wanted to reuse this method fixed true == fixedUpdate is being called
		behaviourSystem->update(*persistentScene, deltaTime, gameWorld, true);
	}
}


void SceneManager::update(double deltaTime, const GameWorld &gameWorld)
{
	if (!activeScene || paused) return;

	behaviourSystem->update(*activeScene, deltaTime, gameWorld, false);
	destroySystem->processQueue(*activeScene);

	if (persistentScene)
	{
		behaviourSystem->update(*persistentScene, deltaTime, gameWorld, false);
		destroySystem->processQueue(*persistentScene);
	}
}

void SceneManager::queueDestroy(GameObject *obj) { destroySystem->queueDestroy(obj); }

void SceneManager::shutdown(GameWorld &gameWorld)
{
	destroySystem->clear();

	if (activeScene) { activeScene = nullptr; }

	gameWorld.sceneManager = nullptr;
	scenes.clear();
	persistentScene.reset();
}

const std::string SceneManager::getName() const { return "SceneManager"; }

void SceneManager::configureNetworking(NetworkSpawnManager *spawnMgr)
{
	spawnManager = spawnMgr;
	networkSystem->configure(spawnMgr, behaviourSystem.get());
	networkConfigured = true;
}

bool SceneManager::isNetworkConfigured() const { return networkConfigured; }

bool SceneManager::addScene(std::unique_ptr<Scene> scene)
{
	if (scene == nullptr)
	{
		std::cerr << "[SceneManager] Error: Attempted to add a null scene\n";
		return false;
	}

	const std::string name = scene->getName();
	if (scenes.contains(name))
	{
		std::cerr << "[SceneManager] Error: Scene with name '" << name
				<< "' already exists\n";
		return false;
	}

	scenes.emplace(name, std::move(scene));
	return true;
}

bool SceneManager::removeScene(const std::string &name)
{
	if (persistentScene != nullptr && persistentScene->getName() == name)
	{
		std::cerr << "[SceneManager] Error: Cannot remove persistent scene\n";
		return false;
	}

	const auto it = scenes.find(name);
	if (it == scenes.end()) { return false; }

	if (it->second.get() == activeScene)
	{
		activeScene->onStop();
		activeScene = nullptr;
		paused = false;
	} else { it->second->onStop(); }

	Scene *sceneToRemove = it->second.get();


	if (networkSystem->isConfigured()) { networkSystem->cleanupNetworkObjectsInScene(*sceneToRemove); }

	scenes.erase(it);
	return true;
}

Scene *SceneManager::getScene(const std::string &name) const
{
	if (persistentScene != nullptr && persistentScene->getName() == name) { return persistentScene.get(); }

	const auto it = scenes.find(name);
	if (it != scenes.end()) { return it->second.get(); }

	return nullptr;
}

bool SceneManager::transferGameObject(const std::string &fromSceneName,
									const std::string &toSceneName,
									const std::string &objectName) const
{
	Scene *fromScene = getScene(fromSceneName);
	Scene *toScene = getScene(toSceneName);

	if (fromScene == nullptr || toScene == nullptr)
	{
		std::cerr << "[SceneManager] Error: Scene not found\n";
		return false;
	}

	if (fromScene->getGameObject(objectName) == nullptr)
	{
		std::cerr << "[SceneManager] Error: GameObject '" << objectName
				<< "' not found in scene '" << fromSceneName << "'\n";
		return false;
	}

	if (toScene->getGameObject(objectName) != nullptr)
	{
		std::cerr << "[SceneManager] Error: GameObject '" << objectName
				<< "' already exists in scene '" << toSceneName << "'\n";
		return false;
	}

	auto gameObject = fromScene->extractGameObject(objectName);
	if (gameObject == nullptr) { return false; }

	gameObject->setBehavioursEnabled(true);
	toScene->addGameObject(std::move(gameObject));
	return true;
}

Scene *SceneManager::getActiveScene() const { return activeScene; }

bool SceneManager::setActiveScene(const std::string &name)
{
	if (activeScene && activeScene->getName() == name)
	{
		std::cout << "[SceneManager] Warning: Scene with name '" << name
				<< "' is already active\n";
		return true;
	}

	Scene *nextScene = getScene(name);
	if (nextScene == nullptr)
	{
		std::cerr << "[SceneManager] Error: Scene with name '" << name
				<< "' not found\n";
		return false;
	}

	if (activeScene != nullptr && activeScene != persistentScene.get())
	{
		behaviourSystem->disableBehaviours(activeScene->getAllComponentsOfType<Behaviour>());
		activeScene->onStop();

		if (networkSystem->isConfigured())
		{
			networkSystem->cleanupNetworkObjectsInScene(*activeScene);
		}
	}


	if (nextScene == persistentScene.get())
	{
		std::cerr << "[SceneManager] Error: Cannot set persistent scene as "
				"active scene\n";
		return false;
	}


	activeScene = nextScene;
	paused = false;

	networkSystem->processSceneForNetwork(*activeScene, *gameWorld);
	behaviourSystem->initialiseScene(*activeScene, *gameWorld);
	activeScene->onStart(*gameWorld);

	std::cout << "[SceneManager] Scene " << name << " now active. Contents:" << std::endl;
	activeScene->forEachGameObject([](GameObject& obj) {
		std::cout << "  - " << obj.getName();
		if (auto* identity = obj.getComponent<NetworkIdentity>()) {
			std::cout << " (netId=" << identity->getNetId() << ")";
		}
		std::cout << std::endl;
	});

	return true;
}

bool SceneManager::loadScene(const std::string &name) { return setActiveScene(name); }

void SceneManager::pause()
{
	if (activeScene == nullptr || paused) { return; }

	paused = true;
	activeScene->onStop();
}

void SceneManager::resume()
{
	if (activeScene == nullptr || !paused) { return; }

	paused = false;
	behaviourSystem->enableBehaviours(activeScene->getAllComponentsOfType<Behaviour>());
}

bool SceneManager::isPaused() const { return paused; }

Scene *SceneManager::getOrCreatePersistentScene()
{
	if (persistentScene == nullptr)
	{
		persistentScene = std::make_unique<Scene>("__PersistentScene__");
		behaviourSystem->initialiseScene(*persistentScene, *gameWorld);
	}

	return persistentScene.get();
}

Scene *SceneManager::getPersistentScene() const { return persistentScene.get(); }

std::string SceneManager::getFirstSceneName() const
{
	if (!scenes.empty()) { return scenes.begin()->first; }
	return "";
}

void SceneManager::applyNetworkSnapshot(
	std::string currentSceneName,
	std::vector<std::unique_ptr<GameObject> > &receivedObjects)
{
	if (!activeScene) return;

	if (activeScene->getName() != currentSceneName)
	{
		std::cout << "[SceneManager] Switching scene to " << currentSceneName << std::endl;
		setActiveScene(currentSceneName);
	}

	networkSystem->applyNetworkSnapshot(
		currentSceneName,
		receivedObjects,
		activeScene,
		*gameWorld
	);
}

BehaviourSystem &SceneManager::getBehaviourSystem() { return *behaviourSystem; }
