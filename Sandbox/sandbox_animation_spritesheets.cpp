#include "Animation/AnimationClip.h"
#include "Animation/AnimationCurve.h"
#include "Animation/AnimationTrack.h"
#include "Animation/Animator.h"
#include "Animation/SpritesheetAnimationClip.h"
#include "Assets/AssetManager.h"
#include "Assets/SpritesheetLoader.h"
#include "Behaviours/PlayerControllerBehaviour.h"
#include "Component/SpriteComponent.h"
#include "Component/Transform.h"
#include "External/SdlContext.h"
#include "GameObject/GameObject.h"
#include "GameObject/Vector2.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Rendering/Color.h"
#include "Rendering/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

int main()
{
	SdlContext context(SDL_INIT_VIDEO);
	SDLRenderer renderer(context);

	WindowOptions options{"Player Game", SCREEN_WIDTH, SCREEN_HEIGHT};
	renderer.open(options);
	if ( !renderer.isOpen() )
	{
		std::cout << "Failed to open SDL window\n";
		return 1;
	}

	SceneManager sceneManager;
	sceneManager.setClearColor(Color::darkGray());

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
		assetManager.get(), playerSprite, "Sandbox/Assets/player.png", 4, 6, 64,
		64);

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
		assetManager.get(), ratSprite, "Sandbox/Assets/rat.png", 1, 4, 32, 32);

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
		// Store clip as a static variable so it persists for the lifetime of
		// main()
		static AnimationClip ratMoveClip("RatMove", true);
		AnimationCurve ratCurve(EasingType::EaseInOutQuad);

		Vector2 ratStartPos{200.0f, SCREEN_HEIGHT / 2.0f};
		Vector2 ratEndPos{800.0f, SCREEN_HEIGHT / 2.0f};
		float ratMoveDuration = 0.16f;

		// Clear any existing tracks (in case this runs multiple times)
		ratMoveClip.getTracks().clear();

		// Forward track (plays from 0 to ratMoveDuration)
		AnimationTrack ratForwardTrack(TargetType::Transform,
									   PropertyType::Position, ratMoveDuration,
									   false, ratStartPos, ratEndPos, ratCurve);
		ratMoveClip.addTrack(ratForwardTrack);

		// Reverse track (plays from ratMoveDuration to 2*ratMoveDuration)
		AnimationTrack ratReverseTrack(TargetType::Transform,
									   PropertyType::Position, ratMoveDuration,
									   false, ratEndPos, ratStartPos, ratCurve);
		ratMoveClip.addTrack(ratReverseTrack);

		// FlipX tracks: face right during forward movement, left during reverse
		// Forward flip track (0 to ratMoveDuration): facing right (FlipX = 0)
		AnimationTrack ratFlipForwardTrack(TargetType::Sprite,
										   PropertyType::FlipX, ratMoveDuration,
										   false, 0, 0, ratCurve);
		ratMoveClip.addTrack(ratFlipForwardTrack);

		// Reverse flip track (ratMoveDuration to 2*ratMoveDuration): facing
		// left (FlipX = 1)
		AnimationTrack ratFlipReverseTrack(TargetType::Sprite,
										   PropertyType::FlipX, ratMoveDuration,
										   false, 1, 1, ratCurve);
		ratMoveClip.addTrack(ratFlipReverseTrack);

		// Add sprite frame animation tracks to the same clip
		// This allows sprite frames and transform to animate simultaneously
		SpritesheetAnimationClip ratWalkClip;
		ratWalkClip.name = "walk";
		ratWalkClip.frames = {0, 1, 2, 3};
		ratWalkClip.frameDuration = 0.1f;
		ratWalkClip.loop = true;
		ratAnimator->addSpritesheetTracksToClip(&ratMoveClip, ratWalkClip);

		// Play the combined clip (sprite frames + transform animation)
		// The clip length will be 2 * ratMoveDuration = 6.0 seconds
		ratAnimator->play(&ratMoveClip);

		std::cout << "Rat loaded successfully\n";
		std::cout << "Rat movement clip length: " << ratMoveClip.getLength()
				  << " seconds\n";
		std::cout << "Rat start pos: (" << ratStartPos.x() << ", "
				  << ratStartPos.y() << ")\n";
		std::cout << "Rat end pos: (" << ratEndPos.x() << ", " << ratEndPos.y()
				  << ")\n";
	}
	else
	{
		std::cout << "Warning: Failed to load rat.png\n";
	}

	// Add GameObjects to scene
	gameScene->addGameObject(std::move(player));
	gameScene->addGameObject(std::move(rat));

	sceneManager.addScene(std::move(gameScene));
	sceneManager.setActiveScene("GameScene");

	std::cout << "\n=== Player Game ===\n";
	std::cout << "Controls:\n";
	std::cout << "  WASD or Arrow Keys - Move player\n";
	std::cout << "  ESC - Exit\n\n";

	bool running = true;
	Uint32 lastTicks = SDL_GetTicks();
	RenderQueue renderQueue;
	InputManager* input = InputManager::getInstance();

	while ( running && renderer.isOpen() )
	{
		input->update();

		// Calculate deltaTime
		Uint32 currentTicks = SDL_GetTicks();
		float deltaTime =
			static_cast<float>(currentTicks - lastTicks) / 1000.0f;
		lastTicks = currentTicks;

		// Clamp deltaTime to prevent large jumps
		if ( deltaTime > 0.1f )
		{
			deltaTime = 0.1f;
		}

		// Handle input for exit
		if ( input->quitRequested() || input->wasKeyPressed(KeyCode::ESCAPE) )
		{
			running = false;
		}

		sceneManager.update(deltaTime);
		sceneManager.buildRenderQueue(renderQueue);
		executeRenderQueue(renderer, renderQueue);

		SDL_Delay(16);
	}

	renderer.close();
	return 0;
}
