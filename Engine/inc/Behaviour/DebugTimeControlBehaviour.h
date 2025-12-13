#pragma once

#include "Behaviour/Behaviour.h"
#include "Input/KeyCode.h"

class InputManager;
class ApplicationClock;
class GameWorld;

/**
 * @brief Behaviour component for debug time controls (pause, time scale).
 *
 * Provides configurable keyboard controls for debugging:
 * - Toggle pause/unpause
 * - Adjust simulation time scale (slow motion, fast motion)
 *
 * This component runs every frame (even when paused) via updateAlways(),
 * allowing debug controls to work regardless of simulation state.
 *
 * @example
 * // Use default key bindings
 * gameObject->addComponent<DebugTimeControlBehaviour>();
 *
 * // Use custom key bindings
 * gameObject->addComponent<DebugTimeControlBehaviour>(
 *     KeyCode::P,  // Custom pause key
 *     KeyCode::NUMBER_1_AND_EXCLAMATION,
 *     KeyCode::NUMBER_2_AND_AT,
 *     KeyCode::NUMBER_3_AND_HASHMARK,
 *     KeyCode::NUMBER_4_AND_DOLLAR,
 *     KeyCode::NUMBER_5_AND_PERCENTAGE,
 *     false  // Disable menu printing
 * );
 */
class DebugTimeControlBehaviour: public Behaviour
{
public:
	/**
	 * @brief Constructs a DebugTimeControlBehaviour with configurable key bindings.
	 *
	 * @param pauseKey Key to toggle pause/unpause (default: SPACE)
	 * @param normalSpeedKey Key for normal speed 1.0x (default: 1)
	 * @param slowKey Key for slow motion 0.5x (default: 2)
	 * @param verySlowKey Key for very slow motion 0.25x (default: 3)
	 * @param fastKey Key for fast motion 2.0x (default: 4)
	 * @param veryFastKey Key for very fast motion 4.0x (default: 5)
	 * @param printMenu Whether to print control menu on startup (default: true)
	 */
	explicit DebugTimeControlBehaviour(
		KeyCode pauseKey = KeyCode::SPACE,
		KeyCode normalSpeedKey = KeyCode::NUMBER_1_AND_EXCLAMATION,
		KeyCode slowKey = KeyCode::NUMBER_2_AND_AT,
		KeyCode verySlowKey = KeyCode::NUMBER_3_AND_HASHMARK,
		KeyCode fastKey = KeyCode::NUMBER_4_AND_DOLLAR,
		KeyCode veryFastKey = KeyCode::NUMBER_5_AND_PERCENTAGE,
		bool printMenu = true
	);

	~DebugTimeControlBehaviour() override = default;

	void onAwake() override;
	void update(float deltaTime, GameWorld* world) override;

private:
	InputManager* inputManager;
	ApplicationClock* clock;
	double currentTimeScale;
	bool menuPrinted;

	// Configurable key bindings
	KeyCode pauseKey;
	KeyCode normalSpeedKey;
	KeyCode slowKey;
	KeyCode verySlowKey;
	KeyCode fastKey;
	KeyCode veryFastKey;
	bool printMenu;
};

