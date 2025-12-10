//
// Created by Lieven Schokker on 07/11/2025.
//

#include "EntryPoint.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include "EntryPoint.h"
#include "Component/ShapeRenderer.h"
#include "Component/Profiler/Profiler.h"
#include "Component/UIElement/UIPanelElement.h"
#include "Component/UIElement/UIProgressBar.h"
#include "Component/UIElement/UISpacer.h"
#include "Demo/PlayerMovement.h"

#undef main

int main(int argc, char** argv)
{
    // Parse command line arguments
    EngineMode mode = EngineMode::CLIENT;  // Default to client

    if (argc > 1)
    {
        std::string arg = argv[1];
        if (arg == "server")
        {
            mode = EngineMode::SERVER;
            std::cout << "Starting as SERVER..." << std::endl;
        }
        else if (arg == "client")
        {
            mode = EngineMode::CLIENT;
            std::cout << "Starting as CLIENT..." << std::endl;
        }
        else
        {
            std::cerr << "Unknown mode: " << arg << std::endl;
            std::cerr << "Usage: " << argv[0] << " [server|client]" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "No mode specified, defaulting to CLIENT..." << std::endl;
        std::cout << "Usage: " << argv[0] << " [server|client]" << std::endl;
    }

    SceneManager sceneManager = SceneManager();
    ApplicationSpecifications spec = {};
    spec.networkingOptions.mode = mode;
    spec.networkingOptions.port = 8080;
    spec.networkingOptions.serverIP = "127.0.0.1";
    spec.networkingOptions.tickRate = 60;
    spec.renderBackend = RenderBackend::SDL;
    spec.windowOptions = {
        mode == EngineMode::SERVER ? "GameEngine - Server" : "GameEngine - Client",
        700,
        700
    };

    std::unique_ptr<Game> spel = std::make_unique<Game>();
    std::unique_ptr<Scene> scene = std::make_unique<Scene>("SpelScene");

    // Panel
    std::unique_ptr<GameObject> profiler = std::make_unique<GameObject>();
    profiler->addComponent<Profiler>(480.0f, 10.0f, 210.0f, 320.0f);

    std::unique_ptr<GameObject> player = std::make_unique<GameObject>();
    player->addComponent<PlayerMovement>();
    auto* shape = player->addComponent<ShapeRenderer>();
    shape->setCircle(50);
    scene->addGameObject(std::move(player));
    scene->addGameObject(std::move(profiler));
    spel->addScene(std::move(scene));
    spel->setApplicationSpecifications(spec);
    return SpelMotorEntry::main(std::move(spel));
}