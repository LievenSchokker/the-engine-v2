#include "Game.h"

#include "GameObject/GameObject.h"

Game::Game() : sceneManager(std::make_unique<SceneManager>()) {};
Game::~Game() = default;

void Game::setApplicationSpecifications(
	const ApplicationSpecifications& specificationS)
{
	specifications = specificationS;
}

ApplicationSpecifications Game::getApplicationSpecifications() const
{
	return specifications;
}

void Game::addScene(std::unique_ptr<Scene> scene)
{
	std::string name = scene->getName();
	sceneManager->addScene(std::move(scene));

	if ( sceneManager->getActiveScene() == nullptr )
	{
		sceneManager->setActiveScene(name);
	}
}

bool Game::setActiveScene(const std::string& name) const
{
	if ( sceneManager == nullptr )
	{
		return false;
	}
	return sceneManager->setActiveScene(name);
}

void Game::addToPersistentScene(std::unique_ptr<GameObject> gameObject)
{
	if ( sceneManager == nullptr )
	{
		return;
	}
	Scene* persistentScene = sceneManager->getOrCreatePersistentScene();
	if ( persistentScene != nullptr )
	{
		persistentScene->addGameObject(std::move(gameObject));
	}
}

std::unique_ptr<SceneManager> Game::getSceneManager()
{
	return std::move(sceneManager);
}