#include "Animation/AnimationClip.h"
#include "Animation/AnimationCurve.h"
#include "Animation/AnimationTrack.h"
#include "Animation/Animator.h"
#include "Animation/SpritesheetAnimationClip.h"
#include "Assets/AssetManager.h"
#include "Assets/SpritesheetLoader.h"
#include "Behaviour/Behaviour.h"
#include "Behaviour/DebugTimeControlBehaviour.h"
#include "Behaviours/PlayerControllerBehaviour.h"
#include "Component/SpriteComponent.h"
#include "Component/Transform.h"
#include "../Engine/inc/Core/Options/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Math/Vector2.h"
#include "Scene/Scene.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

/**
 * @brief Simple behavior to handle ESC key for exit.
 *
 * Signals quit when ESC is pressed. Window close events are handled
 * automatically by the engine.
 */
class ExitBehaviour: public Behaviour
{
   public:
	ExitBehaviour() : inputManager(nullptr)
	{
	}

	~ExitBehaviour() override = default;

	void onAwake() override
	{
		inputManager = InputManager::getInstance();
	}

	void update(float deltaTime, GameWorld* world) override
	{
		(void)deltaTime;
		(void)world;

		if ( inputManager != nullptr &&
			 inputManager->wasKeyPressed(KeyCode::ESCAPE) )
		{
			// Signal quit to the engine
			inputManager->signalQuit();
		}
	}

   private:
	InputManager* inputManager;
};

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
	spec.windowOptions = {"Player Game", SCREEN_WIDTH, SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	// Create AssetManager for loading spritesheets
	std::unique_ptr<AssetManager> assetManager =
		std::make_unique<AssetManager>();

	// Create a scene
	auto gameScene = std::make_unique<Scene>("GameScene");

	// Create Player GameObject
	auto player = std::make_unique<GameObject>();
	player->setName("Player");
	player->getTransform()->setPosition(
		{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f});
	player->getTransform()->setScale({2.0f, 2.0f});	 // Scale up for visibility

	// Load player spritesheet
	// player.png is 384x256 pixels with 24 frames (6 columns, 4 rows)
	// Each frame is 64x64 pixels (384/6 = 64, 256/4 = 64)
	// Rows: Down (0-5), Right (6-11), Left (12-17), Up (18-23)
	auto* playerSprite = player->addComponent<SpriteComponent>();
	bool playerLoaded = SpritesheetLoader::loadSpritesheet(
		assetManager.get(), playerSprite, "Assets/player.png", 4, 6, 64, 64);

	if ( playerLoaded )
	{
		// Create Animator for player animations
		auto* playerAnimator = player->addComponent<Animator>();

		// Define player animations based on the spritesheet layout
		// Row 0 (frames 0-5): Down/Forward
		// Row 1 (frames 6-11): Left
		// Row 2 (frames 12-17): Right
		// Row 3 (frames 18-23): Up/Backward

		// Idle animations (first frame of each direction)
		SpritesheetAnimationClip idleDownClip;
		idleDownClip.name = "idleDown";
		idleDownClip.frames = {0};
		idleDownClip.frameDuration = 0.2f;
		idleDownClip.loop = true;
		playerAnimator->addSpritesheetClip("idleDown", idleDownClip);

		SpritesheetAnimationClip idleRightClip;
		idleRightClip.name = "idleRight";
		idleRightClip.frames = {12};
		idleRightClip.frameDuration = 0.2f;
		idleRightClip.loop = true;
		playerAnimator->addSpritesheetClip("idleRight", idleRightClip);

		SpritesheetAnimationClip idleLeftClip;
		idleLeftClip.name = "idleLeft";
		idleLeftClip.frames = {6};
		idleLeftClip.frameDuration = 0.2f;
		idleLeftClip.loop = true;
		playerAnimator->addSpritesheetClip("idleLeft", idleLeftClip);

		SpritesheetAnimationClip idleUpClip;
		idleUpClip.name = "idleUp";
		idleUpClip.frames = {18};
		idleUpClip.frameDuration = 0.2f;
		idleUpClip.loop = true;
		playerAnimator->addSpritesheetClip("idleUp", idleUpClip);

		// Walk animations (all frames in each row)
		SpritesheetAnimationClip walkDownClip;
		walkDownClip.name = "walkDown";
		walkDownClip.frames = {0, 1, 2, 3, 4, 5};
		walkDownClip.frameDuration = 0.1f;
		walkDownClip.loop = true;
		playerAnimator->addSpritesheetClip("walkDown", walkDownClip);

		SpritesheetAnimationClip walkRightClip;
		walkRightClip.name = "walkRight";
		walkRightClip.frames = {12, 13, 14, 15, 16, 17};
		walkRightClip.frameDuration = 0.1f;
		walkRightClip.loop = true;
		playerAnimator->addSpritesheetClip("walkRight", walkRightClip);

		SpritesheetAnimationClip walkLeftClip;
		walkLeftClip.name = "walkLeft";
		walkLeftClip.frames = {6, 7, 8, 9, 10, 11};
		walkLeftClip.frameDuration = 0.1f;
		walkLeftClip.loop = true;
		playerAnimator->addSpritesheetClip("walkLeft", walkLeftClip);

		SpritesheetAnimationClip walkUpClip;
		walkUpClip.name = "walkUp";
		walkUpClip.frames = {18, 19, 20, 21, 22, 23};
		walkUpClip.frameDuration = 0.1f;
		walkUpClip.loop = true;
		playerAnimator->addSpritesheetClip("walkUp", walkUpClip);

		// Start with idle down animation
		playerAnimator->playSpritesheet("idleDown");

		// Add player controller behaviour for movement and animation switching
		auto* playerController =
			player->addComponent<PlayerControllerBehaviour>();
		playerController->setSpeed(200.0f);	 // pixels per second

		std::cout << "Player loaded successfully\n";
	}
	else
	{
		std::cout << "Warning: Failed to load player.png\n";
	}

	// Create Rat GameObject with ping-pong movement
	auto rat = std::make_unique<GameObject>();
	rat->setName("Rat");
	rat->getTransform()->setPosition({200.0f, SCREEN_HEIGHT / 2.0f});
	rat->getTransform()->setScale({2.0f, 2.0f});

	// Load rat spritesheet (rat.png - 128x32, 4 frames horizontally)
	auto* ratSprite = rat->addComponent<SpriteComponent>();
	bool ratLoaded = SpritesheetLoader::loadSpritesheet(
		assetManager.get(), ratSprite, "Assets/rat.png", 1, 4, 32, 32);

	if ( ratLoaded )
	{
		// Create single Animator for rat (handles both sprite frames and
		// transform)
		auto* ratAnimator = rat->addComponent<Animator>();

		// Verify sprite component exists
		if ( ratSprite != nullptr )
		{
			std::cout << "Rat sprite component found, frame count: "
					  << ratSprite->getFrameCount() << "\n";
		}
		else
		{
			std::cout << "Warning: Rat sprite component is null!\n";
		}

		// Create ping-pong position animation for the rat
		// Create clip with ownership
		auto ratMoveClip = std::make_unique<AnimationClip>("RatMove", true);
		AnimationCurve ratCurve(EasingType::EaseInOutQuad);

		Vector2 ratStartPos{200.0f, SCREEN_HEIGHT / 2.0f};
		Vector2 ratEndPos{800.0f, SCREEN_HEIGHT / 2.0f};
		float ratMoveDuration = 0.16f;

		// Forward track (plays from 0 to ratMoveDuration)
		AnimationTrack ratForwardTrack(TargetType::Transform,
									   PropertyType::Position, ratMoveDuration,
									   false, ratStartPos, ratEndPos, ratCurve);
		ratMoveClip->addTrack(ratForwardTrack);

		// Reverse track (plays from ratMoveDuration to 2*ratMoveDuration)
		AnimationTrack ratReverseTrack(TargetType::Transform,
									   PropertyType::Position, ratMoveDuration,
									   false, ratEndPos, ratStartPos, ratCurve);
		ratMoveClip->addTrack(ratReverseTrack);

		// FlipX tracks: face right during forward movement, left during reverse
		// Forward flip track (0 to ratMoveDuration): facing right (FlipX = 0)
		AnimationTrack ratFlipForwardTrack(TargetType::Sprite,
										   PropertyType::FlipX, ratMoveDuration,
										   false, 0, 0, ratCurve);
		ratMoveClip->addTrack(ratFlipForwardTrack);

		// Reverse flip track (ratMoveDuration to 2*ratMoveDuration): facing
		// left (FlipX = 1)
		AnimationTrack ratFlipReverseTrack(TargetType::Sprite,
										   PropertyType::FlipX, ratMoveDuration,
										   false, 1, 1, ratCurve);
		ratMoveClip->addTrack(ratFlipReverseTrack);

		// Add sprite frame animation tracks to the same clip
		// This allows sprite frames and transform to animate simultaneously
		// The Animator takes ownership of the clip
		SpritesheetAnimationClip ratWalkClip;
		ratWalkClip.name = "walk";
		ratWalkClip.frames = {0, 1, 2, 3};
		ratWalkClip.frameDuration = 0.1f;
		ratWalkClip.loop = true;
		AnimationClip* ratMoveClipPtr = ratAnimator->addSpritesheetTracksToClip(
			std::move(ratMoveClip), ratWalkClip);

		// Play the combined clip (sprite frames + transform animation)
		// The clip length will be 2 * ratMoveDuration = 6.0 seconds
		ratAnimator->play(ratMoveClipPtr);

		std::cout << "Rat loaded successfully\n";
		std::cout << "Rat movement clip length: " << ratMoveClipPtr->getLength()
				  << " seconds\n";
		std::cout << "Rat start pos: (" << ratStartPos.x << ", "
				  << ratStartPos.y << ")\n";
		std::cout << "Rat end pos: (" << ratEndPos.x << ", " << ratEndPos.y
				  << ")\n";
	}
	else
	{
		std::cout << "Warning: Failed to load rat.png\n";
	}

	// Create a GameObject for exit handling
	auto exitHandler = std::make_unique<GameObject>();
	exitHandler->setName("ExitHandler");
	exitHandler->addComponent<ExitBehaviour>();

	// Create a debug behavior
	auto debugController = std::make_unique<GameObject>();
	debugController->setName("DebugController");
	debugController->addComponent<DebugTimeControlBehaviour>(
		KeyCode::SPACE,					 // Pause key
		std::nullopt,					 // Normal speed (disabled)
		KeyCode::NUMBER_2_AND_AT,		 // Slow (enabled)
		KeyCode::NUMBER_3_AND_HASHMARK,	 // Very slow (enabled)
		KeyCode::NUMBER_4_AND_DOLLAR,	 // Fast (enabled)
		std::nullopt,					 // Very fast (disabled)
		true							 // Print menu
	);

	gameScene->addGameObject(std::move(debugController));

	// Add GameObjects to scene
	gameScene->addGameObject(std::move(player));
	gameScene->addGameObject(std::move(rat));
	gameScene->addGameObject(std::move(exitHandler));

	game->addScene(std::move(gameScene));
	game->setApplicationSpecifications(spec);

	std::cout << "\n=== Player Game ===\n";
	std::cout << "Controls:\n";
	std::cout << "  WASD or Arrow Keys - Move player\n";
	std::cout << "  ESC - Exit\n\n";

	return SpelMotorEntry::main(std::move(game));
}
