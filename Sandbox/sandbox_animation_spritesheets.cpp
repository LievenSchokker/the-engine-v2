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
#include "Component/Camera.h"
#include "Component/SpriteComponent.h"
#include "Component/Transform.h"
#include "Core/GameWorld.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Math/Vector2.h"
#include "Scene/Scene.h"

#include <iostream>
#include <memory>

// Screen dimensions in pixels (logical resolution)
constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;

// Physics/world scale
constexpr float PIXELS_PER_METER = 50.0f;

// World dimensions in meters
constexpr float WORLD_WIDTH = SCREEN_WIDTH / PIXELS_PER_METER;   // 20.48 meters
constexpr float WORLD_HEIGHT = SCREEN_HEIGHT / PIXELS_PER_METER; // 15.36 meters

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
    }

    void update(double deltaTime, const GameWorld& world) override
    {
        inputManager = world.input;

        (void)deltaTime;
        (void)world;

        if (inputManager != nullptr &&
            inputManager->wasKeyPressed(KeyCode::ESCAPE))
        {
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
    spec.engineSystem = EngineSystem::Client;
    spec.renderBackend = RenderBackend::SDL;
    spec.windowOptions = {"Player Game", SCREEN_WIDTH, SCREEN_HEIGHT};
    spec.maxFrameTime = 0.1;

    std::unique_ptr<Game> game = std::make_unique<Game>();

    std::unique_ptr<AssetManager> assetManager =
        std::make_unique<AssetManager>();

    auto gameScene = std::make_unique<Scene>("GameScene");

    // Create Player GameObject
    auto player = std::make_unique<GameObject>();
    player->setName("Player");
	player->addComponent<Camera>(0.1, Vector2{0.0, 0.0}, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Position in meters (center of world)
    player->getTransform()->setPosition({WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f});

    // Scale: sprite is 64x64 pixels, we want it to be ~2 meters tall
    // 2 meters * 50 PPM = 100 pixels, so scale = 100/64 ≈ 1.5625
    // Or simpler: if 1 unit = 1 meter in world space, scale by desired meter size
    constexpr float PLAYER_SIZE_METERS = 2.0f;
    constexpr float PLAYER_FRAME_SIZE = 64.0f;
    float playerScale = PLAYER_SIZE_METERS / (PLAYER_FRAME_SIZE / PIXELS_PER_METER);
    player->getTransform()->setScale({playerScale, playerScale});

    auto* playerSprite = player->addComponent<SpriteComponent>();
    bool playerLoaded = SpritesheetLoader::loadSpritesheet(
        assetManager.get(), playerSprite, "Assets/player.png", 4, 6, 64, 64);

    if (playerLoaded)
    {
        auto* playerAnimator = player->addComponent<Animator>();

        // Idle animations
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

        // Walk animations
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

        playerAnimator->playSpritesheet("idleDown");

        auto* playerController =
            player->addComponent<PlayerControllerBehaviour>();

        // Speed in meters per second (was 200 pixels/sec = 4 meters/sec at 50 PPM)
        playerController->setSpeed(4.0f);

        std::cout << "Player loaded successfully\n";
    }
    else
    {
        std::cout << "Warning: Failed to load player.png\n";
    }

    // Create Rat GameObject
    auto rat = std::make_unique<GameObject>();
    rat->setName("Rat");

    // Position in meters
    rat->getTransform()->setPosition({4.0f, WORLD_HEIGHT / 2.0f});

    // Scale: rat sprite is 32x32, we want it ~1 meter tall
    constexpr float RAT_SIZE_METERS = 1.0f;
    constexpr float RAT_FRAME_SIZE = 32.0f;
    float ratScale = RAT_SIZE_METERS / (RAT_FRAME_SIZE / PIXELS_PER_METER);
    rat->getTransform()->setScale({ratScale, ratScale});

    auto* ratSprite = rat->addComponent<SpriteComponent>();
    bool ratLoaded = SpritesheetLoader::loadSpritesheet(
        assetManager.get(), ratSprite, "Assets/rat.png", 1, 4, 32, 32);

    if (ratLoaded)
    {
        auto* ratAnimator = rat->addComponent<Animator>();

        if (ratSprite != nullptr)
        {
            std::cout << "Rat sprite component found, frame count: "
                      << ratSprite->getFrameCount() << "\n";
        }
        else
        {
            std::cout << "Warning: Rat sprite component is null!\n";
        }

        auto ratMoveClip = std::make_unique<AnimationClip>("RatMove", true);
        AnimationCurve ratCurve(EasingType::EaseInOutQuad);

        // Positions in meters
        Vector2 ratStartPos{4.0f, WORLD_HEIGHT / 2.0f};
        Vector2 ratEndPos{16.0f, WORLD_HEIGHT / 2.0f};
        float ratMoveDuration = 3.0f; // 3 seconds to cross ~12 meters

        // Forward track
        AnimationTrack ratForwardTrack(
            TargetType::Transform, PropertyType::Position, ratMoveDuration,
            false, ratStartPos, ratEndPos, ratCurve);
        ratMoveClip->addTrack(ratForwardTrack);

        // Reverse track
        AnimationTrack ratReverseTrack(
            TargetType::Transform, PropertyType::Position, ratMoveDuration,
            false, ratEndPos, ratStartPos, ratCurve);
        ratMoveClip->addTrack(ratReverseTrack);

        // FlipX tracks
        AnimationTrack ratFlipForwardTrack(
            TargetType::Sprite, PropertyType::FlipX, ratMoveDuration, false,
            0, 0, ratCurve);
        ratMoveClip->addTrack(ratFlipForwardTrack);

        AnimationTrack ratFlipReverseTrack(
            TargetType::Sprite, PropertyType::FlipX, ratMoveDuration, false,
            1, 1, ratCurve);
        ratMoveClip->addTrack(ratFlipReverseTrack);

        SpritesheetAnimationClip ratWalkClip;
        ratWalkClip.name = "walk";
        ratWalkClip.frames = {0, 1, 2, 3};
        ratWalkClip.frameDuration = 0.1f;
        ratWalkClip.loop = true;
        AnimationClip* ratMoveClipPtr =
            ratAnimator->addSpritesheetTracksToClip(std::move(ratMoveClip),
                                                    ratWalkClip);

        ratAnimator->play(ratMoveClipPtr);

        std::cout << "Rat loaded successfully\n";
        std::cout << "Rat movement clip length: "
                  << ratMoveClipPtr->getLength() << " seconds\n";
        std::cout << "Rat start pos: (" << ratStartPos.x << ", "
                  << ratStartPos.y << ") meters\n";
        std::cout << "Rat end pos: (" << ratEndPos.x << ", " << ratEndPos.y
                  << ") meters\n";
    }
    else
    {
        std::cout << "Warning: Failed to load rat.png\n";
    }

    // Exit handler
    auto exitHandler = std::make_unique<GameObject>();
    exitHandler->setName("ExitHandler");
    exitHandler->addComponent<ExitBehaviour>();

    // Debug controller
    auto debugController = std::make_unique<GameObject>();
    debugController->setName("DebugController");
    debugController->addComponent<DebugTimeControlBehaviour>(
        KeyCode::SPACE,
        std::nullopt,
        KeyCode::NUMBER_2_AND_AT,
        KeyCode::NUMBER_3_AND_HASHMARK,
        KeyCode::NUMBER_4_AND_DOLLAR,
        std::nullopt,
        true
    );

    gameScene->addGameObject(std::move(debugController));
    gameScene->addGameObject(std::move(player));
    gameScene->addGameObject(std::move(rat));
    gameScene->addGameObject(std::move(exitHandler));

    game->addScene(std::move(gameScene));
    game->setApplicationSpecifications(spec);

    std::cout << "\n=== Player Game ===\n";
    std::cout << "World size: " << WORLD_WIDTH << "x" << WORLD_HEIGHT << " meters\n";
    std::cout << "Pixels per meter: " << PIXELS_PER_METER << "\n";
    std::cout << "Controls:\n";
    std::cout << "  WASD or Arrow Keys - Move player\n";
    std::cout << "  ESC - Exit\n\n";

    return SpelMotorEntry::main(std::move(game));
}