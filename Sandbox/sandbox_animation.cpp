#include "Animation/AnimationClip.h"
#include "Animation/AnimationCurve.h"
#include "Animation/AnimationTrack.h"
#include "Animation/Animator.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "External/SdlContext.h"
#include "GameObject/GameObject.h"
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

	WindowOptions options{"Animation Example", SCREEN_WIDTH, SCREEN_HEIGHT};
	renderer.open(options);
	if ( !renderer.isOpen() )
	{
		std::cout << "Failed to open SDL window\n";
		return 1;
	}

	SceneManager sceneManager;
	sceneManager.setClearColor(Color::darkGray());

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
	auto* movingCircleAnimator = movingCircle->addComponent<Animator>();
	movingCircleAnimator->setAnimationSystem(sceneManager.getAnimationSystem());
	movingCircleAnimator->play(&positionClip);

	auto* rotatingSquareAnimator = rotatingSquare->addComponent<Animator>();
	rotatingSquareAnimator->setAnimationSystem(
		sceneManager.getAnimationSystem());
	rotatingSquareAnimator->play(&rotationClip);

	auto* pulsingRectAnimator = pulsingRect->addComponent<Animator>();
	pulsingRectAnimator->setAnimationSystem(sceneManager.getAnimationSystem());
	pulsingRectAnimator->play(&scaleClip);

	auto* combinedAnimator = combinedObject->addComponent<Animator>();
	combinedAnimator->setAnimationSystem(sceneManager.getAnimationSystem());
	combinedAnimator->play(&combinedClip);

	auto* pingPongAnimator = pingPongObject->addComponent<Animator>();
	pingPongAnimator->setAnimationSystem(sceneManager.getAnimationSystem());
	pingPongAnimator->play(&pingPongClip);

	// Add GameObjects to scene
	animationScene->addGameObject(std::move(movingCircle));
	animationScene->addGameObject(std::move(rotatingSquare));
	animationScene->addGameObject(std::move(pulsingRect));
	animationScene->addGameObject(std::move(combinedObject));
	animationScene->addGameObject(std::move(pingPongObject));

	sceneManager.addScene(std::move(animationScene));
	sceneManager.setActiveScene("AnimationScene");

	std::cout << "Animation System Demo\n";
	std::cout << "=====================\n";
	std::cout << "Controls:\n";
	std::cout << "  SPACE - Pause/Resume all animations\n";
	std::cout << "  R - Reset all animations\n";
	std::cout << "  T - Toggle time scale (normal/slow/fast)\n";
	std::cout << "  ESC - Exit\n\n";

	bool running = true;
	Uint32 lastTicks = SDL_GetTicks();
	RenderQueue renderQueue;
	InputManager* input = InputManager::getInstance();
	Scene* activeScene = sceneManager.getActiveScene();

	// Get animators for controls
	GameObject* movingCircleObj = activeScene->getGameObject("MovingCircle");
	GameObject* rotatingSquareObj =
		activeScene->getGameObject("RotatingSquare");
	GameObject* pulsingRectObj = activeScene->getGameObject("PulsingRect");
	GameObject* combinedObj = activeScene->getGameObject("CombinedAnimation");
	GameObject* pingPongObj = activeScene->getGameObject("PingPongAnimation");

	Animator* movingCircleAnim =
		movingCircleObj ? movingCircleObj->getComponent<Animator>() : nullptr;
	Animator* rotatingSquareAnim =
		rotatingSquareObj ? rotatingSquareObj->getComponent<Animator>()
						  : nullptr;
	Animator* pulsingRectAnim =
		pulsingRectObj ? pulsingRectObj->getComponent<Animator>() : nullptr;
	Animator* combinedAnim =
		combinedObj ? combinedObj->getComponent<Animator>() : nullptr;
	Animator* pingPongAnim =
		pingPongObj ? pingPongObj->getComponent<Animator>() : nullptr;

	float timeScaleState = 1.0f;  // 1.0 = normal, 0.5 = slow, 2.0 = fast

	while ( running && renderer.isOpen() )
	{
		input->update();

		// Pause/Resume all animations
		if ( input->wasKeyPressed(KeyCode::SPACE) )
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
		if ( input->wasKeyPressed(KeyCode::R) )
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
		if ( input->wasKeyPressed(KeyCode::T) )
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

		if ( input->quitRequested() || input->wasKeyPressed(KeyCode::ESCAPE) )
		{
			running = false;
		}

		Uint32 currentTicks = SDL_GetTicks();
		float deltaTime =
			static_cast<float>(currentTicks - lastTicks) / 1000.0f;
		lastTicks = currentTicks;

		sceneManager.update(deltaTime);
		sceneManager.buildRenderQueue(renderQueue);
		executeRenderQueue(renderer, renderQueue);

		SDL_Delay(16);
	}

	renderer.close();
	return 0;
}
