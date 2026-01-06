#include "Core/Options/ApplicationSpecifications.h"
#include "Behaviour/Behaviour.h"
#include "Behaviour/DebugTimeControlBehaviour.h"
#include "Behaviours/SimpleMoveBehaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Core/GameWorld.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Rendering/RenderSystem.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/SaveSceneSystem.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 700

/// Path for saving/loading scenes
static const std::string SAVE_PATH = "saves/";
static const std::string SAVE_FILENAME = "quicksave.scene";

/**
 * @brief Behavior class that handles input for the sandbox scene.
 *
 * Handles:
 * - SPACE key: Toggles clear color between dark green and dark purple
 * - R key: Rotates the YellowRectangle
 * - S key: Saves the current scene
 * - L key: Loads the last saved scene
 * - Mouse wheel Y: Rotates the YellowRectangle
 * - Mouse wheel X: Moves and scales the BlueCircle
 */
class SandboxInputBehaviour final: public Behaviour, RegistrationBase<SandboxInputBehaviour>
{
   public:
	SandboxInputBehaviour() = default;
	~SandboxInputBehaviour() override = default;

	void onAwake() override
	{
		std::cout << "Sandbox Awake" << std::endl;
	}

	static constexpr const char* name()
	{
		return "SandboxInputBehaviour";
	}

	const char* getName() const override { return name(); }

	void onEnable() override
	{
		std::cout << "Sandbox Enable" << std::endl;
		std::cout << "R - Rotate YellowRectangle" << std::endl;
		std::cout << "S - Save current scene" << std::endl;
		std::cout << "L - Load last saved scene" << std::endl;
		std::cout << "SPACE - Toggle background color" << std::endl;
		std::cout << "ENTER - Switch scenes" << std::endl;
	}

	void serialize(WriteArchive& archive) const override
	{
		std::cout << "Sandbox Serialize" << std::endl;
	}
	void deserialize(ReadArchive& archive) override
	{
		std::cout << "Sandbox Deserialize" << std::endl;
	}
	void update(double deltaTime, const GameWorld& world) override
	{
	    (void)deltaTime;
		Scene* scene = world.sceneManager->getActiveScene();
		if ( scene == nullptr )
		{
			return;
		}

		if ( world.input->wasKeyPressed(KeyCode::R) )
		{
			GameObject* rectangle = scene->getGameObject("YellowRectangle");
			if ( rectangle != nullptr )
			{
				double currentRotation = rectangle->getTransform()->getRotationAngle();
				rectangle->getTransform()->setRotationAngle(currentRotation + 15.0);
			}
		}

		if ( world.input->wasKeyPressed(KeyCode::S) )
		{
			std::string savePath = SAVE_PATH + SAVE_FILENAME;

			if ( SaveSceneSystem::saveScene(*scene, savePath) )
			{
				std::cout << "Scene saved successfully to: " << savePath << std::endl;
			}
			else
			{
				std::cout << "Failed to save scene!" << std::endl;
			}
		}

		if ( world.input->wasKeyPressed(KeyCode::L) )
		{
			std::string savePath = SAVE_PATH + SAVE_FILENAME;

			if ( !SaveSceneSystem::isValidSceneFile(savePath) )
			{
				std::cout << "[Load] No valid save file found at: " << savePath << std::endl;
				return;
			}

			auto loadedScene = SaveSceneSystem::loadScene(savePath);
			if ( loadedScene != nullptr )
			{
				std::string sceneName = loadedScene->getName();
				SceneManager* sm = world.sceneManager;
				sm->removeScene(sceneName);
				sm->addScene(std::move(loadedScene));
				sm->setActiveScene(sceneName);
				std::cout << "[Load] Scene loaded successfully from: " << savePath << std::endl;
			}
			else
			{
				std::cout << "[Load] Failed to load scene!" << std::endl;
			}
		}

		if ( world.input->wasKeyPressed(KeyCode::E) )
		{
			auto circle = scene->getGameObject("BlueCircle");
			GameObject* rectangle = scene->getGameObject("YellowRectangle");
			if ( rectangle != nullptr )
			{
				rectangle->getTransform()->setRotationAngle(rectangle->getTransform()->getRotationAngle() + 5.0);
			}
		}

		if ( world.input->wasKeyPressed(KeyCode::SPACE) )
		{
			Color clearColor = world.render->getClearColor();
			clearColor = (clearColor == Color::darkGreen())
							 ? Color::darkPurple()
							 : Color::darkGreen();

			if ( world.render != nullptr )
			{
				world.render->setClearColor(clearColor);
			}
		}

		if ( world.input->wasKeyPressed(KeyCode::ENTER) )
		{
			SceneManager* sm = world.sceneManager;
			Scene* active = sm->getActiveScene();

			if ( active->getName() == "PrototypeScene" )
				sm->setActiveScene("SecondScene");
			else
			{
				sm->loadScene("PrototypeScene");
			}
		}
	}
};

//////////////////////////////
/// Create Scenes
//////////////////////////////

std::unique_ptr<Scene> createPrototypeScene()
{
	auto scene = std::make_unique<Scene>("PrototypeScene");

	auto circle = std::make_unique<GameObject>();
	circle->setName("BlueCircle");
	circle->getTransform()->setPosition({150.0, 140.0});
	circle->getTransform()->setScale({1.0, 1.0});
	circle->addComponent<ShapeRenderer>()->setCircle(50.0).setColor(
		Color::lightBlue());
	circle->addComponent<SimpleMoveBehaviour>();

	auto rectangle = std::make_unique<GameObject>();
	rectangle->setName("YellowRectangle");
	rectangle->getTransform()->setPosition({320.0, 240.0});
	rectangle->getTransform()->setRotationAngle(25.0);
	rectangle->getTransform()->setScale({1.0, 1.0});
	rectangle->addComponent<ShapeRenderer>()
		->setRectangle({140.0, 80.0})
		.setColor(Color::lightRed());

	scene->addGameObject(std::move(circle));
	scene->addGameObject(std::move(rectangle));

	return scene;
}

std::unique_ptr<Scene> createSecondScene()
{
	auto scene = std::make_unique<Scene>("SecondScene");

	auto rectA = std::make_unique<GameObject>();
	rectA->setName("RectA");
	rectA->getTransform()->setPosition({200.0f, 200.0f});
	rectA->addComponent<ShapeRenderer>()
		->setRectangle({140.0, 80.0})
		.setColor(Color::yellow());
	scene->addGameObject(std::move(rectA));

	auto rectB = std::make_unique<GameObject>();
	rectB->setName("RectB");
	rectB->getTransform()->setPosition({500.0f, 200.0f});
	rectB->addComponent<ShapeRenderer>()
		->setRectangle({140.0, 80.0})
		.setColor(Color::green());
	scene->addGameObject(std::move(rectB));

	return scene;
}

#undef main

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	ApplicationSpecifications spec = {};
	spec.networkingOptions.port = 8080;
	spec.networkingOptions.serverIP = "127.0.0.1";
	spec.networkingOptions.mode = EngineMode::CLIENT;
	spec.networkingOptions.tickRate = 60;
	spec.renderBackend = RenderBackend::SDL;
	spec.engineSystem = EngineSystem::Client;
	spec.windowOptions = {"Shape Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	auto prototypeScene = createPrototypeScene();
	auto secondScene = createSecondScene();

	// Create a debug controller and add it directly to the persistent scene
	// This ensures debug controls work across all scene transitions
	auto debugController = std::make_unique<GameObject>();
	debugController->setName("DebugController");
	debugController->addComponent<DebugTimeControlBehaviour>(
		KeyCode::NUMBER_1_AND_EXCLAMATION,	// Pause key
		std::nullopt,						// Normal speed (disabled)
		KeyCode::NUMBER_2_AND_AT,			// Slow (enabled)
		KeyCode::NUMBER_3_AND_HASHMARK,		// Very slow (enabled)
		KeyCode::NUMBER_4_AND_DOLLAR,		// Fast (enabled)
		std::nullopt,						// Very fast (disabled)
		true								// Print menu
	);

	prototypeScene->addGameObject(std::move(debugController));

	// Attach behavior
	auto goInputHandler = std::make_unique<GameObject>();
	goInputHandler->setName("InputHandler");
	goInputHandler->addComponent<SandboxInputBehaviour>();
	prototypeScene->addGameObject(std::move(goInputHandler));
	game->addScene(std::move(prototypeScene));

	// Attach behavior
	auto goInputHandler2 = std::make_unique<GameObject>();
	goInputHandler2->setName("InputHandler");
	goInputHandler2->addComponent<SandboxInputBehaviour>();
	secondScene->addGameObject(std::move(goInputHandler2));
	game->addScene(std::move(secondScene));

	// game->addScene(createSecondScene());
	game->setApplicationSpecifications(spec);

	return SpelMotorEntry::main(std::move(game));
}