//
// Created by Lieven Schokker on 07/11/2025.
//

#include "EntryPoint.h"
#include "Scene/SceneManager.h"


#include <iostream>
#include "EntryPoint.h"
#include "Component/Profiler/Profiler.h"
#include "Component/UIElement/UIPanelElement.h"
#include "Component/UIElement/UIProgressBar.h"
#include "Component/UIElement/UISpacer.h"

#undef main

int main(int argc, char** argv)
{

    SceneManager sceneManager = SceneManager();
	ApplicationSpecifications spec = {};
	spec.networkingOptions.port = 8080;
	spec.networkingOptions.serverIP = "127.0.0.1";
	spec.networkingOptions.tickRate = 60;
	spec.renderBackend = RenderBackend::SDL;
	spec.windowOptions = {"GameEngine", 700, 700};

	std::unique_ptr<Game> spel = std::make_unique<Game>();
	std::unique_ptr<Scene> scene = std::make_unique<Scene>("SpelScene");

	// Panel
	std::unique_ptr<GameObject> profiler = std::make_unique<GameObject>();
	profiler->addComponent<Profiler>(480.0f, 10.0f, 210.0f, 320.0f);  // Top-right of 700x700 window

	scene->addGameObject(std::move(profiler));
	spel->addScene(std::move(scene));
	spel->setApplicationSpecifications(spec);
	return SpelMotorEntry::main(std::move(spel));
}