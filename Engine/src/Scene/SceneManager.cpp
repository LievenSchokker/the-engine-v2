#include "Scene/SceneManager.h"

#include "GameObject/GameObject.h"

#include <iostream>
#include <utility>

bool SceneManager::addScene(std::unique_ptr<Scene> scene)
{
	if ( scene == nullptr )
	{
		std::cerr << "[SceneManager] Error: Attempted to add a null scene\n";
		return false;
	}

	const std::string name = scene->getName();
	if ( scenes.contains(name) )
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

	if ( fromScene == nullptr || toScene == nullptr )
	{
		std::cerr << "[SceneManager] Error: Scene not found\n";
		return false;
	}

	// Check if object exists in source scene
	if ( fromScene->getGameObject(objectName) == nullptr )
	{
		std::cerr << "[SceneManager] Error: GameObject '" << objectName
				  << "' not found in scene '" << fromSceneName << "'\n";
		return false;
	}

	// Check if object already exists in target scene
	if ( toScene->getGameObject(objectName) != nullptr )
	{
		std::cerr << "[SceneManager] Error: GameObject '" << objectName
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
	if ( activeScene && activeScene->getName() == name )
	{
		std::cout << "[SceneManager] Warning: Scene with name '" << name
				  << "' is already active\n";
		return true;
	}

	Scene* nextScene = getScene(name);
	if ( nextScene == nullptr )
	{
		std::cerr << "[SceneManager] Error: Scene with name '" << name
				  << "' not found\n";
		return false;
	}

	if ( activeScene != nullptr )
	{
		activeScene->onStop();
	}

	activeScene = nextScene;
	paused = false;
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

void SceneManager::fixedUpdate(double deltaTime)
{
	if ( activeScene != nullptr && !paused )
	{
		activeScene->fixedUpdate(deltaTime);
	}
}
