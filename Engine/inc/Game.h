#pragma once


#include "Core/ApplicationSpecifications.h"
#include "Scene/Scene.h"

class Game
{
public:
	Game();
	~Game();
	[[nodiscard]] ApplicationSpecifications getApplicationSpecifications() const;
	void setApplicationSpecifications(const ApplicationSpecifications& specifications);
	void addScene(std::unique_ptr<Scene> scene);
private:
	ApplicationSpecifications specifications;
	std::vector<std::unique_ptr<Scene>> scenes;
};