#include "PlayerControllerBehaviour.h"

#include "Animation/Animator.h"
#include "Component/Transform.h"

#include <cmath>

PlayerControllerBehaviour::PlayerControllerBehaviour()
	: inputManager(nullptr),
	  animator(nullptr),
	  movementSpeed(200.0f),
	  lastDirection(0),
	  directionThreshold(0.3f),
	  lastPosition(Vector2::zero())
{
}

void PlayerControllerBehaviour::onAwake()
{
	inputManager = InputManager::getInstance();
	animator = gameObject->getComponent<Animator>();
}

void PlayerControllerBehaviour::onStart()
{
	// Store initial position for movement delta calculation
	if ( transform != nullptr )
	{
		lastPosition = transform->getPosition();
	}
}

void PlayerControllerBehaviour::update(float deltaTime)
{
	if ( inputManager == nullptr || transform == nullptr )
	{
		return;
	}

	handleInput(deltaTime);
	updateAnimation();
}

void PlayerControllerBehaviour::setSpeed(float speed)
{
	movementSpeed = speed;
}

float PlayerControllerBehaviour::getSpeed() const
{
	return movementSpeed;
}

void PlayerControllerBehaviour::handleInput(float deltaTime)
{
	if ( transform == nullptr )
	{
		return;
	}

	Vector2 currentPos = transform->getPosition();
	Vector2 newPos = currentPos;
	bool isMoving = false;

	// Check input and move player
	if ( inputManager->isKeyDown(KeyCode::W) ||
		 inputManager->isKeyDown(KeyCode::UP_ARROW) )
	{
		newPos.setY(newPos.y() - movementSpeed * deltaTime);
		isMoving = true;
	}
	if ( inputManager->isKeyDown(KeyCode::S) ||
		 inputManager->isKeyDown(KeyCode::DOWN_ARROW) )
	{
		newPos.setY(newPos.y() + movementSpeed * deltaTime);
		isMoving = true;
	}
	if ( inputManager->isKeyDown(KeyCode::A) ||
		 inputManager->isKeyDown(KeyCode::LEFT_ARROW) )
	{
		newPos.setX(newPos.x() - movementSpeed * deltaTime);
		isMoving = true;
	}
	if ( inputManager->isKeyDown(KeyCode::D) ||
		 inputManager->isKeyDown(KeyCode::RIGHT_ARROW) )
	{
		newPos.setX(newPos.x() + movementSpeed * deltaTime);
		isMoving = true;
	}

	// Update player position
	transform->setPosition(newPos);

	// Store position for next frame's delta calculation
	lastPosition = currentPos;
}

void PlayerControllerBehaviour::updateAnimation()
{
	if ( animator == nullptr || transform == nullptr )
	{
		return;
	}

	Vector2 currentPos = transform->getPosition();
	float dx = currentPos.x() - lastPosition.x();
	float dy = currentPos.y() - lastPosition.y();
	float absDx = std::abs(dx);
	float absDy = std::abs(dy);
	float totalMovement = absDx + absDy;

	bool isMoving = totalMovement > 0.001f;

	if ( isMoving )
	{
		// Determine desired direction based on movement delta
		int desiredDirection = determineDirection(dx, dy);

		// Only switch animation if direction changed
		if ( desiredDirection != lastDirection )
		{
			lastDirection = desiredDirection;

			switch ( desiredDirection )
			{
				case 1:	 // up
					if ( animator->getCurrentSpritesheetClipName() != "walkUp" )
					{
						animator->playSpritesheet("walkUp");
					}
					break;
				case 2:	 // down
					if ( animator->getCurrentSpritesheetClipName() !=
						 "walkDown" )
					{
						animator->playSpritesheet("walkDown");
					}
					break;
				case 3:	 // left
					if ( animator->getCurrentSpritesheetClipName() !=
						 "walkLeft" )
					{
						animator->playSpritesheet("walkLeft");
					}
					break;
				case 4:	 // right
					if ( animator->getCurrentSpritesheetClipName() !=
						 "walkRight" )
					{
						animator->playSpritesheet("walkRight");
					}
					break;
				default:
					// Invalid direction - should not happen, but handle
					// gracefully
					break;
			}
		}
	}
	else
	{
		// Not moving - switch to idle based on last direction
		lastDirection = 0;	// Reset direction tracking
		std::string currentClip = animator->getCurrentSpritesheetClipName();
		if ( currentClip == "walkRight" )
		{
			animator->playSpritesheet("idleRight");
		}
		else if ( currentClip == "walkLeft" )
		{
			animator->playSpritesheet("idleLeft");
		}
		else if ( currentClip == "walkUp" )
		{
			animator->playSpritesheet("idleUp");
		}
		else if ( currentClip != "idleDown" && currentClip != "idleRight" &&
				  currentClip != "idleLeft" && currentClip != "idleUp" )
		{
			// Default to idle down if we're not already in an idle state
			animator->playSpritesheet("idleDown");
		}
	}
}

int PlayerControllerBehaviour::determineDirection(float dx, float dy)
{
	if ( inputManager == nullptr )
	{
		return 0;
	}

	// Get input state
	bool upPressed = inputManager->isKeyDown(KeyCode::W) ||
					 inputManager->isKeyDown(KeyCode::UP_ARROW);
	bool downPressed = inputManager->isKeyDown(KeyCode::S) ||
					   inputManager->isKeyDown(KeyCode::DOWN_ARROW);
	bool leftPressed = inputManager->isKeyDown(KeyCode::A) ||
					   inputManager->isKeyDown(KeyCode::LEFT_ARROW);
	bool rightPressed = inputManager->isKeyDown(KeyCode::D) ||
						inputManager->isKeyDown(KeyCode::RIGHT_ARROW);

	float absDx = std::abs(dx);
	float absDy = std::abs(dy);
	float totalMovement = absDx + absDy;

	int desiredDirection = 0;

	if ( totalMovement > 0.001f )  // Avoid division by zero
	{
		float horizontalRatio = absDx / totalMovement;
		float verticalRatio = absDy / totalMovement;

		// Use hysteresis: only switch if the ratio difference is significant
		// enough, or if we're clearly moving in one direction
		if ( horizontalRatio > verticalRatio + directionThreshold )
		{
			// Clearly horizontal movement
			desiredDirection = (dx > 0) ? 4 : 3;  // right : left
		}
		else if ( verticalRatio > horizontalRatio + directionThreshold )
		{
			// Clearly vertical movement
			desiredDirection = (dy > 0) ? 2 : 1;  // down : up
		}
		else
		{
			// Diagonal movement - use last direction if valid, otherwise
			// prioritize horizontal
			if ( lastDirection >= 1 && lastDirection <= 4 )
			{
				// Keep last direction to prevent jittery switching
				desiredDirection = lastDirection;
			}
			else
			{
				// No previous direction - prioritize horizontal (common in
				// games)
				desiredDirection = (dx > 0) ? 4 : 3;
			}
		}
	}
	else
	{
		// Fallback: determine from input keys directly
		if ( rightPressed && !leftPressed )
		{
			desiredDirection = 4;  // right
		}
		else if ( leftPressed && !rightPressed )
		{
			desiredDirection = 3;  // left
		}
		else if ( downPressed && !upPressed )
		{
			desiredDirection = 2;  // down
		}
		else if ( upPressed && !downPressed )
		{
			desiredDirection = 1;  // up
		}
		else
		{
			// Multiple keys - use last direction or default
			desiredDirection =
				(lastDirection != 0) ? lastDirection : 2;  // default to down
		}
	}

	return desiredDirection;
}
