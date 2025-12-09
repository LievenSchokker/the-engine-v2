
#include "Component/Profiler/Profiler.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Component/UIElement/UIPanelElement.h"
#include "Component/UIElement/UIProgressBar.h"
#include "Component/UIElement/UISpacer.h"
#include "Core/ApplicationSpecifications.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Rendering/Color.h"
#include "Scene/Scene.h"

#include <iostream>

#undef main

int main(int argc, char** argv)
{
	ApplicationSpecifications spec = {};
	spec.networkingOptions.port = 8080;
	spec.networkingOptions.serverIP = "127.0.0.1";
	spec.networkingOptions.mode = EngineMode::CLIENT;
	spec.networkingOptions.tickRate = 60;
	spec.renderBackend = RenderBackend::SDL;
	spec.windowOptions = {"GameEngine", 700, 700};

	std::unique_ptr<Game> spel = std::make_unique<Game>();
	std::unique_ptr<Scene> scene = std::make_unique<Scene>("Scene");

	// Put a blue circle on the
	std::unique_ptr<GameObject> circle = std::make_unique<GameObject>();
	circle->setName("BlueCircle");
	circle->getTransform()->setPosition({350.0, 350.0});
	circle->getTransform()->setScale({1.0, 1.0});
	circle->addComponent<ShapeRenderer>()->setCircle(50.0).setColor(
		Color::lightBlue());
	scene->addGameObject(std::move(circle));

	spel->addScene(std::move(scene));
	spel->setApplicationSpecifications(spec);
	return SpelMotorEntry::main(std::move(spel));
}