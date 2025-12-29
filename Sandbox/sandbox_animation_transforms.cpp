#include "Animation/AnimationClip.h"
#include "Animation/AnimationCurve.h"
#include "Animation/AnimationTrack.h"
#include "Animation/Animator.h"
#include "Behaviour/Behaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Rendering/Color.h"
#include "Scene/Scene.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

/**
 * @brief Behavior class that handles input for the animation demo.
 *
 * Handles:
 * - SPACE: Pause/Resume all animations
 * - R: Reset all animations
 * - T: Toggle time scale (normal/slow/fast)
 */
class AnimationInputBehaviour: public Behaviour
{
   public:
	explicit AnimationInputBehaviour(Scene* scene)
		: scene(scene), inputManager(nullptr), timeScaleState(1.0f)
	{
	}

	~AnimationInputBehaviour() override = default;

	void onAwake() override
	{
		// Get all animators from the scene
		if ( scene != nullptr )
		{
			GameObject* movingCircleObj = scene->getGameObject("MovingCircle");
			GameObject* rotatingSquareObj =
				scene->getGameObject("RotatingSquare");
			GameObject* pulsingRectObj = scene->getGameObject("PulsingRect");
			GameObject* combinedObj = scene->getGameObject("CombinedAnimation");
			GameObject* pingPongObj = scene->getGameObject("PingPongAnimation");

			if ( movingCircleObj )
				movingCircleAnim = movingCircleObj->getComponent<Animator>();
			if ( rotatingSquareObj )
				rotatingSquareAnim =
					rotatingSquareObj->getComponent<Animator>();
			if ( pulsingRectObj )
				pulsingRectAnim = pulsingRectObj->getComponent<Animator>();
			if ( combinedObj )
				combinedAnim = combinedObj->getComponent<Animator>();
			if ( pingPongObj )
				pingPongAnim = pingPongObj->getComponent<Animator>();
		}

		std::cout << "Animation System Demo\n";
		std::cout << "=====================\n";
		std::cout << "Controls:\n";
		std::cout << "  SPACE - Pause/Resume all animations\n";
		std::cout << "  R - Reset all animations\n";
		std::cout << "  T - Toggle time scale (normal/slow/fast)\n";
		std::cout << "  ESC - Exit\n\n";
	}

	void update(double deltaTime, const GameWorld& world) override
	{
		inputManager = world.input;
		(void)deltaTime;
		(void)world;

		if ( inputManager == nullptr )
		{
			return;
		}

		// Pause/Resume all animations
		if ( inputManager->wasKeyPressed(KeyCode::SPACE) )
		{
			bool shouldPause =
				movingCircleAnim && movingCircleAnim->getIsPlaying();

			if ( movingCircleAnim )
			{
				if ( shouldPause )
					movingCircleAnim->pause();
				else
					movingCircleAnim->resume();
			}
			if ( rotatingSquareAnim )
			{
				if ( shouldPause )
					rotatingSquareAnim->pause();
				else
					rotatingSquareAnim->resume();
			}
			if ( pulsingRectAnim )
			{
				if ( shouldPause )
					pulsingRectAnim->pause();
				else
					pulsingRectAnim->resume();
			}
			if ( combinedAnim )
			{
				if ( shouldPause )
					combinedAnim->pause();
				else
					combinedAnim->resume();
			}
			if ( pingPongAnim )
			{
				if ( shouldPause )
					pingPongAnim->pause();
				else
					pingPongAnim->resume();
			}

			std::cout << "Animations: " << (shouldPause ? "PAUSED" : "RESUMED")
					  << "\n";
		}

		// Reset all animations
		if ( inputManager->wasKeyPressed(KeyCode::R) )
		{
			if ( movingCircleAnim ) movingCircleAnim->stop();
			if ( rotatingSquareAnim ) rotatingSquareAnim->stop();
			if ( pulsingRectAnim ) pulsingRectAnim->stop();
			if ( combinedAnim ) combinedAnim->stop();
			if ( pingPongAnim ) pingPongAnim->stop();

			// Restart them
			if ( movingCircleAnim && movingCircleAnim->getCurrentClip() )
				movingCircleAnim->play(movingCircleAnim->getCurrentClip());
			if ( rotatingSquareAnim && rotatingSquareAnim->getCurrentClip() )
				rotatingSquareAnim->play(rotatingSquareAnim->getCurrentClip());
			if ( pulsingRectAnim && pulsingRectAnim->getCurrentClip() )
				pulsingRectAnim->play(pulsingRectAnim->getCurrentClip());
			if ( combinedAnim && combinedAnim->getCurrentClip() )
				combinedAnim->play(combinedAnim->getCurrentClip());
			if ( pingPongAnim && pingPongAnim->getCurrentClip() )
				pingPongAnim->play(pingPongAnim->getCurrentClip());

			std::cout << "Animations: RESET\n";
		}

		// Toggle time scale
		if ( inputManager->wasKeyPressed(KeyCode::T) )
		{
			if ( timeScaleState == 1.0f )
			{
				timeScaleState = 0.5f;
				std::cout << "Time scale: SLOW (0.5x)\n";
			}
			else if ( timeScaleState == 0.5f )
			{
				timeScaleState = 2.0f;
				std::cout << "Time scale: FAST (2.0x)\n";
			}
			else
			{
				timeScaleState = 1.0f;
				std::cout << "Time scale: NORMAL (1.0x)\n";
			}

			if ( movingCircleAnim )
				movingCircleAnim->setTimeScale(timeScaleState);
			if ( rotatingSquareAnim )
				rotatingSquareAnim->setTimeScale(timeScaleState);
			if ( pulsingRectAnim )
				pulsingRectAnim->setTimeScale(timeScaleState);
			if ( combinedAnim ) combinedAnim->setTimeScale(timeScaleState);
			if ( pingPongAnim ) pingPongAnim->setTimeScale(timeScaleState);
		}
	}

   private:
	Scene* scene;
	InputManager* inputManager;
	float timeScaleState;  // 1.0 = normal, 0.5 = slow, 2.0 = fast

	// Animator references
	Animator* movingCircleAnim = nullptr;
	Animator* rotatingSquareAnim = nullptr;
	Animator* pulsingRectAnim = nullptr;
	Animator* combinedAnim = nullptr;
	Animator* pingPongAnim = nullptr;
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
    spec.engineSystem = EngineSystem::Client;
	spec.windowOptions = {"Animation Example", SCREEN_WIDTH, SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	// Create a scene
	auto animationScene = std::make_unique<Scene>("AnimationScene");

	// Create GameObjects for different animation demonstrations
	// 1. Position animation - moving circle
	auto movingCircle = std::make_unique<GameObject>();
	movingCircle->setName("MovingCircle");
	movingCircle->getTransform()->setPosition({100.0f, 300.0f});
	auto* circleRenderer = movingCircle->addComponent<ShapeRenderer>();
	circleRenderer->setCircle(30.0f).setColor(Color::lightBlue());

	// 2. Rotation animation - rotating square
	auto rotatingSquare = std::make_unique<GameObject>();
	rotatingSquare->setName("RotatingSquare");
	rotatingSquare->getTransform()->setPosition({300.0f, 300.0f});
	auto* squareRenderer = rotatingSquare->addComponent<ShapeRenderer>();
	squareRenderer->setRectangle({60.0f, 60.0f}).setColor(Color::yellow());

	// 3. Scale animation - pulsing rectangle
	auto pulsingRect = std::make_unique<GameObject>();
	pulsingRect->setName("PulsingRect");
	pulsingRect->getTransform()->setPosition({500.0f, 300.0f});
	auto* rectRenderer = pulsingRect->addComponent<ShapeRenderer>();
	rectRenderer->setRectangle({80.0f, 80.0f}).setColor(Color::purple());

	// 4. Combined animation - all transformations
	auto combinedObject = std::make_unique<GameObject>();
	combinedObject->setName("CombinedAnimation");
	combinedObject->getTransform()->setPosition({700.0f, 300.0f});
	auto* combinedRenderer = combinedObject->addComponent<ShapeRenderer>();
	combinedRenderer->setRectangle({50.0f, 50.0f}).setColor(Color::green());

	// 5. Ping-pong animation - smoothly goes forward then backward
	auto pingPongObject = std::make_unique<GameObject>();
	pingPongObject->setName("PingPongAnimation");
	pingPongObject->getTransform()->setPosition({500.0f, 300.0f});
	auto* pingPongRenderer = pingPongObject->addComponent<ShapeRenderer>();
	pingPongRenderer->setRectangle({40.0f, 40.0f}).setColor(Color::orange());

	// Create AnimationClips
	// 1. Position animation clip (back and forth)
	AnimationClip positionClip("PositionAnimation", true);
	AnimationCurve positionCurve(EasingType::EaseInOutQuad);
	Vector2 startPos{100.0f, 300.0f};
	Vector2 endPos{500.0f, 300.0f};
	AnimationTrack positionTrack(TargetType::Transform, PropertyType::Position,
								 3.0f, false, startPos, endPos, positionCurve);
	positionClip.addTrack(positionTrack);

	// 2. Rotation animation clip (360 degrees)
	AnimationClip rotationClip("RotationAnimation", true);
	AnimationCurve rotationCurve(EasingType::Linear);
	AnimationTrack rotationTrack(TargetType::Transform, PropertyType::Rotation,
								 2.0f, false, 0.0f, 360.0f, rotationCurve);
	rotationClip.addTrack(rotationTrack);

	// 3. Scale animation clip (pulsing)
	AnimationClip scaleClip("ScaleAnimation", true);
	AnimationCurve scaleCurve(EasingType::EaseInOutCubic);
	Vector2 minScale{0.5f, 0.5f};
	Vector2 maxScale{1.5f, 1.5f};
	AnimationTrack scaleTrack(TargetType::Transform, PropertyType::Scale, 1.5f,
							  false, minScale, maxScale, scaleCurve);
	scaleClip.addTrack(scaleTrack);

	// 4. Combined animation clip (position + rotation + scale)
	AnimationClip combinedClip("CombinedAnimation", false);
	AnimationCurve combinedCurve(EasingType::EaseInOutQuad);

	// Position track
	Vector2 combinedStartPos{700.0f, 200.0f};
	Vector2 combinedEndPos{700.0f, 400.0f};
	AnimationTrack combinedPosTrack(
		TargetType::Transform, PropertyType::Position, 2.0f, false,
		combinedStartPos, combinedEndPos, combinedCurve);
	combinedClip.addTrack(combinedPosTrack);

	// Rotation track
	AnimationTrack combinedRotTrack(TargetType::Transform,
									PropertyType::Rotation, 2.0f, false, 0.0f,
									180.0f, combinedCurve);
	combinedClip.addTrack(combinedRotTrack);

	// Scale track
	Vector2 combinedMinScale{0.8f, 0.8f};
	Vector2 combinedMaxScale{1.2f, 1.2f};
	AnimationTrack combinedScaleTrack(
		TargetType::Transform, PropertyType::Scale, 2.0f, false,
		combinedMinScale, combinedMaxScale, combinedCurve);
	combinedClip.addTrack(combinedScaleTrack);

	// 5. Ping-pong animation clip (smoothly reverses instead of teleporting)
	// This clip is twice as long - first half goes forward, second half
	// reverses
	AnimationClip pingPongClip("PingPongAnimation", true);
	AnimationCurve pingPongCurve(EasingType::EaseInOutQuad);

	Vector2 pingPongStartPos{500.0f, 600.0f};
	Vector2 pingPongEndPos{700.0f, 600.0f};
	float pingPongDuration = 2.0f;

	// Forward track (0 to duration)
	AnimationTrack pingPongForwardTrack(
		TargetType::Transform, PropertyType::Position, pingPongDuration, false,
		pingPongStartPos, pingPongEndPos, pingPongCurve);
	pingPongClip.addTrack(pingPongForwardTrack);

	// Reverse track (duration to 2*duration) - goes from end back to start
	AnimationTrack pingPongReverseTrack(
		TargetType::Transform, PropertyType::Position, pingPongDuration, false,
		pingPongEndPos, pingPongStartPos, pingPongCurve);
	pingPongClip.addTrack(pingPongReverseTrack);

	// Add Animators to GameObjects and set up animations
	// Scene will automatically update these animators as Behaviour components
	auto* movingCircleAnimator = movingCircle->addComponent<Animator>();
	movingCircleAnimator->play(&positionClip);

	auto* rotatingSquareAnimator = rotatingSquare->addComponent<Animator>();
	rotatingSquareAnimator->play(&rotationClip);

	auto* pulsingRectAnimator = pulsingRect->addComponent<Animator>();
	pulsingRectAnimator->play(&scaleClip);

	auto* combinedAnimator = combinedObject->addComponent<Animator>();
	combinedAnimator->play(&combinedClip);

	auto* pingPongAnimator = pingPongObject->addComponent<Animator>();
	pingPongAnimator->play(&pingPongClip);

	// Create a GameObject for input handling
	auto inputHandler = std::make_unique<GameObject>();
	inputHandler->setName("InputHandler");
	// Add the behavior with a reference to the scene
	inputHandler->addComponent<AnimationInputBehaviour>(animationScene.get());

	// Add GameObjects to scene
	animationScene->addGameObject(std::move(movingCircle));
	animationScene->addGameObject(std::move(rotatingSquare));
	animationScene->addGameObject(std::move(pulsingRect));
	animationScene->addGameObject(std::move(combinedObject));
	animationScene->addGameObject(std::move(pingPongObject));
	animationScene->addGameObject(std::move(inputHandler));

	game->addScene(std::move(animationScene));
	game->setApplicationSpecifications(spec);

	return SpelMotorEntry::main(std::move(game));
}
