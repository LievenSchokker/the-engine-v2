
#include "Core/Options/ApplicationSpecifications.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Component/Profiler/Profiler.h"

#include "IZandbak.h"
#include "Sandboxes//AgentsZandbak.h"
#include "Sandboxes/PathFindingZandbak.h"
#include "Sandboxes/Multiplayer/MultiplayerZandbak.h"

// This has been added because sometimes SDL causes main to be redefined.
// Which then causes linking error's
#undef main


int main(int argc, char** argv)
{
    // Parse command line arguments
	std::unique_ptr<Game> spel = std::make_unique<Game>();

    ApplicationSpecifications spec = {};
	EngineMode mode = EngineMode::CLIENT;
    spec.networkingOptions.mode = mode;
    spec.networkingOptions.port = 8080;
    spec.networkingOptions.serverIP = "127.0.0.1";
    spec.networkingOptions.tickRate = 60;
    spec.renderBackend = RenderBackend::SDL;
	spec.engineSystem = EngineSystem::Client;
    spec.windowOptions = {
        "GameEngine",
        700,
        700
    };

    /// Note: Change the unique_ptr to create the sandbox you want
    std::unique_ptr<IZandbak> zandbak = std::make_unique<MultiplayerZandbak>();
	std::unique_ptr<Scene> scene = zandbak->getScene();

	spel->addScene(std::move(scene));
	spel->setApplicationSpecifications(spec);
	return SpelMotorEntry::main(std::move(spel));
}
