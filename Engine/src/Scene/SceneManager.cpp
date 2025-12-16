#include "Scene/SceneManager.h"

#include "GameObject/GameObject.h"
#include "Component/Transform.h"
#include "Component/NetworkIdentity.h"
#include "Behaviour/NetworkBehaviour.h"
#include "Networking/NetworkSpawnManager.h"

#include <iostream>
#include <utility>

SceneManager::SceneManager()
    : scenes(std::unordered_map<std::string, std::unique_ptr<Scene>>())
{
}

void SceneManager::configureNetworking(ConnectionMode mode, NetworkSpawnManager* spawnMgr)
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

void SceneManager::processForServer(Scene& scene)
{
	std::vector<GameObject*> networkObjects;

	for (auto& obj : scene.getGameObjects())
	{
		if (hasNetworkBehaviour(*obj) && !hasNetworkIdentity(*obj))
		{
			networkObjects.push_back(obj.get());
		}
	}

	if (networkObjects.empty())
	{
		return;
	}

	for (GameObject* obj : networkObjects)
	{
		Vector2 spawnPosition = obj->getTransform()->getPosition();

		std::unique_ptr<GameObject> extracted = scene.extractGameObject(obj);
		if (!extracted) continue;

		extracted->getTransform()->setPosition({0, 0});

		if (!extracted->getComponent<NetworkIdentity>())
		{
			extracted->addComponent<NetworkIdentity>();
		}

		if (spawnManager)
		{
			uint32_t assetId = spawnManager->addToPrefabLibrary(std::move(extracted));
		}
	}
}

void SceneManager::processForClient(Scene& scene)
{
	// Collect objects to process
	std::vector<GameObject*> toProcess;

	for (auto& obj : scene.getGameObjects())
	{
		// Has NetworkBehaviour but NO NetworkIdentity = network prefab template
		if (hasNetworkBehaviour(*obj) && !hasNetworkIdentity(*obj))
		{
			toProcess.push_back(obj.get());
		}
	}

	if (toProcess.empty())
	{
		return;
	}

	for (GameObject* obj : toProcess)
	{
		std::string name = obj->getName();

		// Extract from scene
		std::unique_ptr<GameObject> extracted = scene.extractGameObject(obj);
		if (!extracted) continue;

		// Reset position for prefab template
		extracted->getTransform()->setPosition({0, 0});

		// Add NetworkIdentity if missing
		if (!extracted->getComponent<NetworkIdentity>())
		{
			extracted->addComponent<NetworkIdentity>();
		}

		// Register as prefab (client needs this to instantiate from SpawnMessage)
		if (spawnManager != nullptr)
		{
			uint32_t assetId = spawnManager->addToPrefabLibrary(std::move(extracted));
		}
	}
}


bool SceneManager::hasNetworkBehaviour(const GameObject& obj) const
{
	for (const auto& component : obj.getComponentManager()->getComponents())
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
    if ( scene == nullptr ) {
        return false;
    }

    const std::string name = scene->getName();
    if ( scenes.contains(name) ) {

        return false;
    }

    scenes.emplace(name, std::move(scene));
    return true;
}

bool SceneManager::removeScene(const std::string& name)
{
	// Prevent removal of persistent scene
	if ( persistentScene != nullptr && persistentScene->getName() == name )
	{
		std::cerr << "[SceneManager] Error: Cannot remove persistent scene\n";
		return false;
	}

	const auto it = scenes.find(name);
	if ( it == scenes.end() )
	{
		return false;
	}

	if ( it->second.get() == activeScene )
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
	const auto it = scenes.find(name);
	if ( it != scenes.end() )
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

	if ( fromScene == nullptr || toScene == nullptr ) {
		std::cerr << "Error: Scene not found\n";
		return false;
	}

	// Check if object exists in source scene
	if ( fromScene->getGameObject(objectName) == nullptr ) {
		std::cerr << "Error: GameObject '" << objectName
				  << "' not found in scene '" << fromSceneName << "'\n";
		return false;
	}

	// Check if object already exists in target scene
	if ( toScene->getGameObject(objectName) != nullptr ) {
		std::cerr << " Error: GameObject '" << objectName
				  << "' already exists in scene '" << toSceneName << "'\n";
		return false;
	}

	// Extract and transfer
	auto gameObject = fromScene->extractGameObject(objectName);
	if ( gameObject == nullptr )
	{
		return false;
	}

	toScene->addGameObject(std::move(gameObject));
	return true;
}

Scene* SceneManager::getActiveScene() const
{
	return activeScene;
}

bool SceneManager::setActiveScene(const std::string& name)
{
	if ( activeScene && activeScene->getName() == name ) {
		std::cout << " Warning: Scene with name '" << name
				  << "' is already active\n";
		return true;
	}

	Scene* nextScene = getScene(name);
	if ( nextScene == nullptr ) {
		std::cerr << "Error: Scene with name '" << name
				  << "' not found\n";
		return false;
	}

	// Don't stop if it's the persistent scene (persistent scene is never
	// stopped)
	if ( activeScene != nullptr && activeScene != persistentScene.get() )
	{
		activeScene->onStop();
	}

	// Don't allow setting persistent scene as active scene
	if ( nextScene == persistentScene.get() )
	{
		std::cerr << "[SceneManager] Error: Cannot set persistent scene as "
					 "active scene\n";
		return false;
	}

	activeScene = nextScene;
	paused = false;
    if (networkConfigured && !processedScenes.contains(name))
    {
        processSceneForNetwork(*activeScene);
        processedScenes.insert(name);
    }

    activeScene->onStart();
    return true;
}

bool SceneManager::loadScene(const std::string& name)
{
	return setActiveScene(name);
}

void SceneManager::pause()
{
	if ( activeScene == nullptr || paused )
	{
		return;
	}

	paused = true;
	activeScene->onPause();
}

void SceneManager::resume()
{
	if ( activeScene == nullptr || !paused )
	{
		return;
	}

	paused = false;
	activeScene->onResume();
}

bool SceneManager::isPaused() const
{
	return paused;
}

void SceneManager::update(float deltaTime, GameWorld* world)
{
	if ( activeScene != nullptr && !paused )
	{
		activeScene->update(deltaTime, world);
	}
}

void SceneManager::updateAlways(float deltaTime, GameWorld* world)
{
	// Update persistent scene first (always active, never stopped)
	if ( persistentScene != nullptr )
	{
		persistentScene->update(deltaTime, world);
	}

	// Update active scene
	if ( activeScene != nullptr )
	{
		activeScene->update(deltaTime, world);
	}
}

Scene* SceneManager::getOrCreatePersistentScene()
{
	if ( persistentScene == nullptr )
	{
		persistentScene = std::make_unique<Scene>("__PersistentScene__");
		persistentScene
			->onStart();  // Start it immediately so it's always active
	}
	return persistentScene.get();
}

Scene* SceneManager::getPersistentScene() const
{
	return persistentScene.get();
}

void SceneManager::applyNetworkSnapshot(const std::vector<std::unique_ptr<GameObject>>& receivedObjects)
{
    if (!spawnManager || !activeScene) return;

    std::unordered_set<uint32_t> receivedNetIds;

    for (const auto& received : receivedObjects)
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
        }
        else
        {
            auto clone = received->clone();
            if (!clone) continue;

            auto* cloneIdentity = clone->getComponent<NetworkIdentity>();
            if (cloneIdentity)
            {
                cloneIdentity->setWorld(gameWorld);
                spawnManager->getNetworkIdentityRegistry().registerIdentity(cloneIdentity);
            }

            GameObject* rawPtr = clone.get();
            activeScene->addGameObject(std::move(clone));
            spawnManager->trackSpawnedObject(netId, rawPtr);

            if (cloneIdentity)
            {
                cloneIdentity->onNetworkSpawn();
            }
        }
    }

    std::vector<uint32_t> toRemove;

    for (auto* identity : spawnManager->getNetworkIdentityRegistry().getAllIdentities())
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

    for (uint32_t netId : toRemove)
    {
        GameObject* object = spawnManager->getObjectByNetId(netId);
        if (!object) continue;

        auto* identity = object->getComponent<NetworkIdentity>();
        if (identity)
        {
            identity->onNetworkDespawn();
            spawnManager->getNetworkIdentityRegistry().unregisterIdentity(identity);
        }

        spawnManager->untrackSpawnedObject(netId);
        activeScene->removeGameObject(object);
    }
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

        if (netBehaviour->getAuthorityType() == AuthorityType::ClientAuthority &&
            identity->getOwnerId() == gameWorld->localClientId)
        {
            return true;
        }
    }

    return false;
}