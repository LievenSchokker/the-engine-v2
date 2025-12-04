#include "Game.h"

Game::Game() = default;
Game::~Game() = default;


void Game::setApplicationSpecifications(const ApplicationSpecifications& specificationS)
{
	specifications = specificationS;
}

ApplicationSpecifications Game::getApplicationSpecifications() const
{
	return specifications;
}

void Game::addScene(std::unique_ptr<Scene> scene)
{
	scenes.push_back(std::move(scene));
}

