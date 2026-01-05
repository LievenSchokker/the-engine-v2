#include "Game.h"

Game::Game() {};
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
    scenes.push_back(std::move(scene));
}

std::unique_ptr<Scene> Game::getFirstScene()
{
	if (scenes.empty())
	{
		return nullptr;
	}

	std::unique_ptr<Scene> first = std::move(scenes.front());
	scenes.erase(scenes.begin());
	return first;
}

std::vector<std::unique_ptr<Scene>> Game::getAllScenes()
{
	return std::move(scenes);
}
