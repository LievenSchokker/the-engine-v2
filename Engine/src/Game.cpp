#include "Game.h"

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

std::unique_ptr<SceneManager> Game::getSceneManager()
{
	return std::move(sceneManager);
}