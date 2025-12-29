#pragma once

#include "Behaviour/Behaviour.h"
#include "Input/InputManager.h"
#include "Math/Vector2.h"

class Animator;
class GameWorld;

/**
 * @class PlayerControllerBehaviour
 * @brief Handles player movement input and animation direction switching.
 *
 * This Behaviour handles WASD/Arrow key input for player movement and
 * intelligently switches between directional animations (walk/idle) based on
 * movement direction. Uses hysteresis to prevent jittery animation switching
 * during diagonal movement.
 */
class PlayerControllerBehaviour: public Behaviour
{
   public:
	PlayerControllerBehaviour();
	~PlayerControllerBehaviour() override = default;

	void onAwake() override;
	void onStart() override;
	void update(double deltaTime, const GameWorld& gameWorld) override;

	/**
	 * @brief Sets the movement speed in pixels per second.
	 * @param speed Movement speed
	 */
	void setSpeed(float speed);

	/**
	 * @brief Gets the current movement speed.
	 * @return Movement speed in pixels per second
	 */
	float getSpeed() const;

   private:
	/**
	 * @brief Handles input polling and updates movement state.
	 */
	void handleInput(float deltaTime);

	/**
	 * @brief Updates animation based on movement direction.
	 * Uses hysteresis to prevent jittery switching during diagonal movement.
	 */
	void updateAnimation();

	/**
	 * @brief Determines the desired movement direction based on input and
	 * movement delta.
	 * @param dx Horizontal movement delta
	 * @param dy Vertical movement delta
	 * @return Direction code: 0=none, 1=up, 2=down, 3=left, 4=right
	 */
	int determineDirection(float dx, float dy);

	InputManager* inputManager;
	Animator* animator;
	float movementSpeed;
	int lastDirection;				 // 0=none, 1=up, 2=down, 3=left, 4=right
	const float directionThreshold;	 // Minimum ratio difference to switch
									 // direction
	Vector2 lastPosition;  // Track position for movement delta calculation
};
