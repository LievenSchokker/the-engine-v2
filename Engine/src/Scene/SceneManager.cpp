#include "Scene/SceneManager.h"

#include "GameObject/GameObject.h"
#include "Component/Transform.h"
#include "Component/NetworkIdentity.h"
#include "Behaviour/NetworkBehaviour.h"
#include "Networking/NetworkSpawnManager.h"

#include <iostream>
#include <utility>

SceneManager::SceneManager(GameWorld& gameWorld)
	: gameWorld(&gameWorld),
	  scenes(std::unordered_map<std::string, std::unique_ptr<Scene>>()),
	  behaviourSystem(std::make_unique<BehaviourSystem>()),
	  destroySystem(std::make_unique<DestroySystem>())
{
}


SystemStatus SceneManager::start(GameWorld& gameWorld)
{
	this->gameWorld = &gameWorld;
	gameWorld.sceneManager = this;
	return SystemStatus::RUNNING;
}

void SceneManager::fixedUpdate(double deltaTime, const GameWorld& gameWorld)
{
	if (!activeScene || paused) return;

	behaviourSystem->update(*activeScene, deltaTime, gameWorld, true);

	if (persistentScene)
	{
		//wanted to reuse this method fixed true == fixedUpdate is being called
		behaviourSystem->update(*persistentScene, deltaTime, gameWorld, true);
	}
}


void SceneManager::update(double deltaTime, const GameWorld& gameWorld)
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

void SceneManager::queueDestroy(GameObject* obj)
{
	destroySystem->queueDestroy(obj);
}

void SceneManager::shutdown(GameWorld& gameWorld)
{
	destroySystem->clear();

	if (activeScene)
	{
		activeScene = nullptr;
	}

	gameWorld.sceneManager = nullptr;
	scenes.clear();
	persistentScene.reset();
}

const std::string SceneManager::getName() const
{
	return "SceneManager";
}

void SceneManager::configureNetworking(ConnectionMode mode,
                                       NetworkSpawnManager* spawnMgr)
{
	networkMode = mode;
	spawnManager = spawnMgr;
	networkConfigured = true;
}

bool SceneManager::isNetworkConfigured() const
{
	return networkConfigured;
}


void SceneManager::processSceneForNetwork(Scene& scene)
{
	if (networkMode == ConnectionMode::Host)
	{
		processForServer(scene);
	}
	else
	{
		processForClient(scene);
	}
}

void SceneManager::processForServer(Scene& scene) const
{
	std::vector<GameObject*> networkObjects;

    scene.forEachGameObject([&](GameObject& obj) {
        if (hasNetworkBehaviour(obj) && !hasNetworkIdentity(obj)) {
            networkObjects.push_back(&obj);
        }
    });

	if (networkObjects.empty())
	{
		return;
	}

    for (const GameObject* obj : networkObjects)
    {
	    auto behaviour = obj->getComponent<NetworkBehaviour>();
		if (behaviour != nullptr && behaviour->getAuthorityType() == AuthorityType::ClientAuthority)
		{
			// ClientAuthority: just register as prefab (clients will spawn these)
			std::unique_ptr<GameObject> extracted = scene.extractGameObject(obj->getGameObjectHandle());
			if (!extracted) continue;

			if (spawnManager)
			{
				spawnManager->addToPrefabLibrary(std::move(extracted));
			}
			continue;
		}

		// ServerAuthority: register as prefab AND spawn into scene
		Vector2 spawnPosition = obj->getTransform()->getPosition();

		std::unique_ptr<GameObject> extracted = scene.extractGameObject(obj->getGameObjectHandle());
		if (!extracted) continue;

		extracted->getTransform()->setPosition(spawnPosition);

		if (!extracted->getComponent<NetworkIdentity>())
		{
			extracted->addComponent<NetworkIdentity>();
		}

		if (spawnManager)
		{
			uint32_t assetId = spawnManager->addToPrefabLibrary(std::move(extracted));
			spawnManager->spawnObject(assetId, -1);
		}
	}
}

void SceneManager::processForClient(Scene& scene)
{
    std::vector<GameObject*> toProcess;

    scene.forEachGameObject([&](GameObject& obj) {
        if (hasNetworkBehaviour(obj) && !hasNetworkIdentity(obj)) {
            toProcess.push_back(&obj);
        }
    });


    if (toProcess.empty())
    {
        return;
    }

    for (const GameObject* obj : toProcess)
    {
        std::string name = obj->getName();

        std::unique_ptr<GameObject> extracted = scene.extractGameObject(obj->getGameObjectHandle());
        if (!extracted) continue;

        extracted->getTransform()->setPosition({0, 0});

        if (!extracted->getComponent<NetworkIdentity>())
        {
            extracted->addComponent<NetworkIdentity>();
        }

        if (spawnManager != nullptr)
        {
            uint32_t assetId = spawnManager->addToPrefabLibrary(std::move(extracted));
        }
    }
}

bool SceneManager::hasNetworkBehaviour(const GameObject& obj) const
{
	for (const auto& component : obj.getComponents())
	{
		if (dynamic_cast<NetworkBehaviour*>(component.get()))
		{
			return true;
		}
	}
	return false;
}

bool SceneManager::hasNetworkIdentity(const GameObject& obj) const
{
    return obj.getComponent<NetworkIdentity>() != nullptr;
}

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

bool SceneManager::removeScene(const std::string& name)
{
	if (persistentScene != nullptr && persistentScene->getName() == name)
	{
		std::cerr << "[SceneManager] Error: Cannot remove persistent scene\n";
		return false;
	}

	const auto it = scenes.find(name);
	if (it == scenes.end())
	{
		return false;
	}

	if (it->second.get() == activeScene)
	{
		activeScene->onStop();
		activeScene = nullptr;
		paused = false;
	}
	else
	{
		it->second->onStop();
	}

	scenes.erase(it);
	return true;
}

Scene* SceneManager::getScene(const std::string& name) const
{
	if (persistentScene != nullptr && persistentScene->getName() == name)
	{
		return persistentScene.get();
	}

	const auto it = scenes.find(name);
	if (it != scenes.end())
	{
		return it->second.get();
	}

	return nullptr;
}

bool SceneManager::transferGameObject(const std::string& fromSceneName,
                                      const std::string& toSceneName,
                                      const std::string& objectName) const
{
	Scene* fromScene = getScene(fromSceneName);
	Scene* toScene = getScene(toSceneName);

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
	if (gameObject == nullptr)
	{
		return false;
	}

	gameObject->setBehavioursEnabled(true);
	toScene->addGameObject(std::move(gameObject));
	return true;
}

Scene* SceneManager::getActiveScene() const
{
	return activeScene;
}

bool SceneManager::setActiveScene(const std::string& name)
{
    if (activeScene && activeScene->getName() == name)
    {
        std::cout << "[SceneManager] Warning: Scene with name '" << name
                  << "' is already active\n";
        return true;
    }

	Scene* nextScene = getScene(name);
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
	}

	if (nextScene == persistentScene.get())
	{
		std::cerr << "[SceneManager] Error: Cannot set persistent scene as "
			"active scene\n";
		return false;
	}

	activeScene = nextScene;
	paused = false;

    if (networkConfigured && !processedScenes.contains(name))
    {
        processSceneForNetwork(*nextScene);
        processedScenes.insert(name);
    }

	activeScene = nextScene;
    behaviourSystem->initialiseScene(*activeScene, *gameWorld);
	activeScene->onStart(*gameWorld);
	return true;
}

bool SceneManager::loadScene(const std::string& name)
{
	return setActiveScene(name);
}

void SceneManager::pause()
{
	if (activeScene == nullptr || paused)
	{
		return;
	}

	paused = true;
	activeScene->onStop();
}

void SceneManager::resume()
{
	if (activeScene == nullptr || !paused)
	{
		return;
	}

	paused = false;
	behaviourSystem->enableBehaviours(activeScene->getAllComponentsOfType<Behaviour>());
}

bool SceneManager::isPaused() const
{
	return paused;
}

Scene* SceneManager::getOrCreatePersistentScene()
{
	std::cout << "[SceneManager] Error: Scene not found\n";

	if (persistentScene == nullptr)
	{
		persistentScene = std::make_unique<Scene>("__PersistentScene__");
		behaviourSystem->initialiseScene(*persistentScene, *gameWorld);
	}

	return persistentScene.get();
}

Scene* SceneManager::getPersistentScene() const
{
	return persistentScene.get();
}

std::string SceneManager::getFirstSceneName() const
{
	if (!scenes.empty())
	{
		return scenes.begin()->first;
	}
	return "";
}

void SceneManager::applyNetworkSnapshot(std::vector<std::unique_ptr<GameObject>>& receivedObjects)
{
	if (!spawnManager || !activeScene) return;

	std::unordered_set<uint32_t> receivedNetIds;
	std::vector<std::pair<GameObject*, uint32_t>> toFixup;

	for (auto& received : receivedObjects)
	{
		if (!received) continue;

		auto* identity = received->getComponent<NetworkIdentity>();

		if (!identity) continue;

		uint32_t netId = identity->getNetId();
		receivedNetIds.insert(netId);

		GameObject* existing = spawnManager->getObjectByNetId(netId);


		if (existing)
		{
			existing->copyStateFrom(*received);

			if (auto parentNetId = existing->consumePendingParentNetId())
			{
				toFixup.push_back({existing, *parentNetId});
			}
		}
		else
		{
			if (!received) continue;

			auto* identity = received->getComponent<NetworkIdentity>();
			if (identity)
			{
				identity->setWorld(gameWorld);
				spawnManager->getNetworkIdentityRegistry().registerIdentity(identity);
			}

			std::vector<Behaviour*> behaviours = received->getAllBehaviours();
			GameObject* rawPtr = received.get();

			if (auto parentNetId = received->consumePendingParentNetId())
			{
				toFixup.push_back({rawPtr, *parentNetId});
			}

			activeScene->addGameObject(std::move(received));
			spawnManager->trackSpawnedObject(netId, rawPtr);
			behaviourSystem->initialiseRuntimeBehaviours(behaviours, *gameWorld);

			if (identity)
			{
				identity->onNetworkSpawn();
			}
			addInlineChildrenRecursive(rawPtr);
		}
	}

	for (auto& [child, parentNetId] : toFixup)
	{
		GameObject* parent = spawnManager->getObjectByNetId(parentNetId);
		if (parent)
		{
			child->setParent(parent);
		}
	}

	std::vector<uint32_t> toRemove;

	for (auto* identity : spawnManager->getNetworkIdentityRegistry().
										getAllIdentities())
	{
		if (!identity) continue;

		uint32_t netId = identity->getNetId();

		if (isLocallyOwned(identity))
		{
			continue;
		}

		if (!receivedNetIds.contains(netId))
		{
			toRemove.push_back(netId);
		}
	}
	for (const uint32_t netId : toRemove)
	{
		GameObject* object = spawnManager->getObjectByNetId(netId);
		if (!object) continue;

		auto* identity = object->getComponent<NetworkIdentity>();
		if (identity)
		{
			identity->onNetworkDespawn();
			spawnManager->getNetworkIdentityRegistry().unregisterIdentity(
				identity);
		}

		spawnManager->untrackSpawnedObject(netId);
		activeScene->removeGameObject(object->getGameObjectHandle());
	}
}

void SceneManager::addInlineChildrenRecursive(GameObject* obj)
{
	for (auto& child : obj->consumeInlineChildren())
	{
		GameObject* childPtr = child.get();
		activeScene->addGameObject(std::move(child));
		addInlineChildrenRecursive(childPtr);
	}
}

BehaviourSystem& SceneManager::getBehaviourSystem()
{
	return *behaviourSystem;
}


bool SceneManager::isLocallyOwned(NetworkIdentity* identity) const
{
	if (!identity || !gameWorld) return false;

	GameObject* go = identity->getGameObject();
	if (!go) return false;

	for (Behaviour* behaviour : go->getAllBehaviours())
	{
		auto* netBehaviour = dynamic_cast<NetworkBehaviour*>(behaviour);
		if (!netBehaviour) continue;

		if (netBehaviour->getAuthorityType() == AuthorityType::ClientAuthority
		    &&
		    identity->getOwnerId() == gameWorld->localClientId)
		{
			return true;
		}
	}

	return false;
}