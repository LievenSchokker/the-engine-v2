
#include <iostream>

#include "EntryPoint.h"
#include "Component/Profiler/Profiler.h"
#include "Component/UIObject/UIPanelObject.h"
#include "Component/UIObject/UIProgressBar.h"
#include "Component/UIObject/UISpacer.h"

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
	std::unique_ptr<Scene> scene = std::make_unique<Scene>("SpelScene");

	// Panel
	std::unique_ptr<GameObject> profiler = std::make_unique<GameObject>();
	profiler->addComponent<Profiler>();

	scene->addGameObject(std::move(profiler));
	spel->addScene(std::move(scene));
	spel->setApplicationSpecifications(spec);
	return SpelMotorEntry::main(std::move(spel));
}