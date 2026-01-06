
#include "Core/Options/ApplicationSpecifications.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Component/Profiler/Profiler.h"

#include "IZandbak.h"
#include "Sandboxes//AgentsZandbak.h"
#include "Sandboxes/PathFindingZandbak.h"
#include "Sandboxes/Multiplayer/MultiplayerPhysicsZandbak.h"
#include "Sandboxes/Multiplayer/MultiplayerZandbak.h"
#include "Sandboxes/Multiplayer/MultiplayerZandbak.h"
#include "Sandboxes/UserInterfaceZandbak.h"

// This has been added because sometimes SDL causes main to be redefined.
// Which then causes linking error's
#undef main


int main(int argc, char** argv)
{
    // Parse command line arguments
    ApplicationSpecifications spec = {};
	std::unique_ptr<Game> spel = std::make_unique<Game>();
    EngineMode mode = EngineMode::CLIENT;
    spec.engineSystem = EngineSystem::Client;
    if (argc > 1)
    {
        std::string arg = argv[1];
        if (arg == "server")
        {
            mode = EngineMode::SERVER;
            spec.engineSystem = EngineSystem::Server;
            std::cout << "Starting as SERVER..." << std::endl;
        }
        else if (arg == "client")
        {
            mode = EngineMode::CLIENT;
            spec.engineSystem = EngineSystem::Client;
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


    spec.networkingOptions.mode = mode;
    spec.networkingOptions.port = 8080;
    spec.networkingOptions.serverIP = "127.0.0.1";
    spec.networkingOptions.tickRate = 60;
    spec.renderBackend = RenderBackend::SDL;
    spec.windowOptions = {
        "GameEngine",
        700,
        700
    };

    /// Note: Change the unique_ptr to create the sandbox you want
    std::unique_ptr<IZandbak> zandbak = std::make_unique<PathFindingZandbak>();
	std::unique_ptr<Scene> scene = zandbak->getScene();

	spel->addScene(std::move(scene));
	spel->setApplicationSpecifications(spec);
	return SpelMotorEntry::main(std::move(spel));
}
