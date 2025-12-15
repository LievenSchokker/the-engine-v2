#pragma once

#include "Behaviour/Behaviour.h"
#include "Input/KeyCode.h"

#include <optional>

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
 * This component runs every frame (even when paused) by overriding
 * shouldRunWhenPaused() to return true, allowing debug controls to work
 * regardless of simulation state.
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
	 * @brief Constructs a DebugTimeControlBehaviour with configurable key
	 * bindings.
	 *
	 * Pass std::nullopt for any time scale key to disable that feature.
	 * Only the pause feature is enabled by default.
	 *
	 * @param pauseKey Key to toggle pause/unpause (default: std::nullopt =
	 * disabled)
	 * @param normalSpeedKey Key for normal speed 1.0x (default: std::nullopt =
	 * disabled)
	 * @param slowKey Key for slow motion 0.5x (default: std::nullopt =
	 * disabled)
	 * @param verySlowKey Key for very slow motion 0.10x (default: std::nullopt
	 * = disabled)
	 * @param fastKey Key for fast motion 2.0x (default: std::nullopt =
	 * disabled)
	 * @param veryFastKey Key for very fast motion 4.0x (default: std::nullopt =
	 * disabled)
	 * @param printMenu Whether to print control menu on startup (default: true)
	 *
	 * @example
	 * // Only pause feature enabled
	 * gameObject->addComponent<DebugTimeControlBehaviour>();
	 *
	 * // Pause + time scale controls
	 * gameObject->addComponent<DebugTimeControlBehaviour>(
	 *     KeyCode::SPACE,
	 *     KeyCode::NUMBER_1_AND_EXCLAMATION,  // Enable normal speed
	 *     KeyCode::NUMBER_2_AND_AT,           // Enable slow
	 *     std::nullopt,                       // Disable very slow
	 *     KeyCode::NUMBER_4_AND_DOLLAR,       // Enable fast
	 *     std::nullopt                        // Disable very fast
	 * );
	 */
	explicit DebugTimeControlBehaviour(
		std::optional<KeyCode> pauseKey = std::nullopt,
		std::optional<KeyCode> normalSpeedKey = std::nullopt,
		std::optional<KeyCode> slowKey = std::nullopt,
		std::optional<KeyCode> verySlowKey = std::nullopt,
		std::optional<KeyCode> fastKey = std::nullopt,
		std::optional<KeyCode> veryFastKey = std::nullopt,
		bool printMenu = true);

	~DebugTimeControlBehaviour() override = default;

	void onAwake() override;
	void update(float deltaTime, GameWorld* world) override;
	bool shouldRunWhenPaused() const override;

   private:
	InputManager* inputManager;
	ApplicationClock* clock;
	bool menuPrinted;
	bool movedToPersistentScene;

	// Configurable key bindings
	std::optional<KeyCode> pauseKey;
	std::optional<KeyCode> normalSpeedKey;
	std::optional<KeyCode> slowKey;
	std::optional<KeyCode> verySlowKey;
	std::optional<KeyCode> fastKey;
	std::optional<KeyCode> veryFastKey;
	bool printMenu;
};
