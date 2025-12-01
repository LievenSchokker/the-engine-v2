    #include "Core/EngineLoops/ClientLoop.h"
#include "Core/ApplicationClock.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Networking/Client.h"
#include "Networking/Server/ServerInformation.h"
#include "Rendering/IRenderer.h"
#include "Rendering/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"


//TODO Create proper factory for each system that needs to be created
ClientLoop::ClientLoop(ApplicationSpecifications applicationSpecifications, GameWorld* gameWorld)
    : sceneManager(std::make_unique<SceneManager>()),
        gameWorld(std::move(gameWorld))
{
    ClockFunction clockFunction = []() {return 1.0;};
    if (applicationSpecifications.renderBackend == RenderBackend::SDL)
    {
        SdlContext context = SdlContext();
        //TODO SDL Injection layer
        ClockFunction clockFunction = []() { return SDL_GetTicks() / 1000.0; };
        renderer = std::make_unique<SDLRenderer>(context);
    }
}

ClientLoop::~SpelMotor()
{
	shutdown();
}

void ClientLoop::start()
{
    renderer->open(applicationSpecifications.windowOptions);
	initializeNetworking();
}

void ClientLoop::update()
{
	RenderQueue renderQueue;

		sceneManager->buildRenderQueue(renderQueue);
		renderer->presentFrame();
        client->poll();
}

void ClientLoop::fixedUpdate()
{
    client->poll();
    InputManager::getInstance()->update();
    sceneManager->update();
    if (InputManager::getInstance()->quitRequested())
    {
        shutdown();
    }
}

void ClientLoop::initializeNetworking()
{
    ServerConnectionInformation serverInfo;
    serverInfo.ip = applicationSpecifications.networkingOptions.serverIP;
    serverInfo.port = applicationSpecifications.networkingOptions.port;
    client->connectToServer(serverInfo);
}




void ClientLoop::shutdown()
{
	InputManager::shutdown();
	renderer->close();
    client->disconnect();
}


SceneManager* ClientLoop::getSceneManager()
{
	if (sceneManager) return sceneManager.get();
	return nullptr;
}