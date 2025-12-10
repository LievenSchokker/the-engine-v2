#include "Scene/SceneManager.h"
#include "GameObject/GameObject.h"

#include <iostream>
#include <utility>

#include "Component/Transform.h"
#include "../../inc/Component/NetworkIdentity.h"
#include "Behaviour/NetworkBehaviour.h"
#include "Networking/NetworkSpawnManager.h"

void SceneManager::configureNetworking(ConnectionMode mode, NetworkSpawnManager* spawnMgr)
{
    networkMode = mode;
    spawnManager = spawnMgr;
    networkConfigured = true;

    std::cout << "[SceneManager] Network configured: "
              << (mode == ConnectionMode::Host ? "SERVER" : "CLIENT")
              << std::endl;
}

PrefabLibrary& SceneManager::getPrefabLibrary()
{
    return prefabLibrary;
}

const PrefabLibrary& SceneManager::getPrefabLibrary() const
{
    return prefabLibrary;
}

bool SceneManager::isNetworkConfigured() const
{
    return networkConfigured;
}

void SceneManager::processSceneForNetwork(Scene& scene)
{
    std::cout << "[SceneManager] Processing scene '" << scene.getName()
              << "' for networking..." << std::endl;

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
    // Collect objects to process (can't modify while iterating)
    std::vector<GameObject*> networkObjects;

    for (auto& obj : scene.getGameObjects())
    {
        // Has NetworkBehaviour but NO NetworkIdentity = needs processing
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
		std::string name = obj->getName();
		Vector2 position = obj->getTransform()->getPosition();

		// Extract from scene
		std::unique_ptr<GameObject> extracted = scene.extractGameObject(obj);
		if (!extracted) continue;

		// Store spawn info before moving
		Vector2 spawnPosition = extracted->getTransform()->getPosition();

		// Reset position for prefab template
		extracted->getTransform()->setPosition({0, 0});

		// Add NetworkIdentity if missing (required for network prefabs)
		if (!extracted->getComponent<NetworkIdentity>())
		{
			extracted->addComponent<NetworkIdentity>();
		}

		// Register as prefab
		uint32_t assetId = prefabLibrary.add(std::move(extracted));


		// Auto-spawn server-owned objects
		if (spawnManager)
		{
			GameObject* spawned = spawnManager->spawnObject(assetId, spawnPosition, -1);
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
		std::cout << "[SceneManager] No network objects to process" << std::endl;
		return;
	}

	for (GameObject* obj : toProcess)
	{
		std::string name = obj->getName();

		std::cout << "[SceneManager] Client: Processing '" << name
				  << "' (extracting as prefab, awaiting server spawn)" << std::endl;

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
		uint32_t assetId = prefabLibrary.add(std::move(extracted));

		std::cout << "[SceneManager] Client: Registered prefab '" << name
				  << "' with assetId=" << assetId << std::endl;
	}

	std::cout << "[SceneManager] Client processing complete. Registered "
			  << prefabLibrary.size() << " prefabs" << std::endl;
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
	const auto it = scenes.find(name);
	if ( it == scenes.end() ) {
		return false;
	}

	if ( it->second.get() == activeScene ) {
		activeScene->onStop();
		activeScene = nullptr;
		paused = false;
	} else {
		it->second->onStop();
	}

	scenes.erase(it);
	return true;
}

Scene* SceneManager::getScene(const std::string& name) const
{
	const auto it = scenes.find(name);
	if ( it != scenes.end() ) {
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
	if ( gameObject == nullptr ) {
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

	if ( activeScene != nullptr ) {
		activeScene->onStop();
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
	if ( activeScene == nullptr || paused ) {
		return;
	}

	paused = true;
	activeScene->onPause();
}

void SceneManager::resume()
{
	if ( activeScene == nullptr || !paused ) {
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
	if (activeScene != nullptr && !paused)
	{
		activeScene->update(deltaTime, world);
	}
}

void SceneManager::setClearColor(const Color& color)
{
	clearColor = color;
}

Color SceneManager::getClearColor() const
{
	return clearColor;
}
