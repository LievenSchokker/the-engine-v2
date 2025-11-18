#pragma once

#include "ApplicationSpecifications.h"
#include "Rendering/IRender.h"

/**
 * @class SpelMotor
 * @brief Core engine class that manages the game loop and system lifecycle.
 *
 * SpelMotor acts as the orchestrator for the entire engine managing
 * the lifetime cycle of all engine systems.
 *
 */
class SpelMotor
{
public:
    SpelMotor(ApplicationSpecifications applicationSpecifications);

    /**
     * @brief Starts the engine and enters the main game loop.
     *
     * This method performs all system initialization (rendering, input, etc.) and then
     * enters the BLOCKING update loop.
     * It only returns when the engine has been shut down.
     *
     */
    void run();

    /**
     * @brief Immediately shuts down all engine systems.
     *
     * Performs cleanup of all subsystems in the reverse order of their initialization
     * to prevent dependency issues.
     *
     */
    void shutdown();

private:
    /**
     * @brief The main game loop that runs until shutdown is requested.
     *
     * Encapsulated as a private method to enforce that the game loop can only be
     * entered through run(), preventing accidental re-entry or misuse.
     *
     */
    void update();

    /**
     * @brief Checks shutdown flag and performs cleanup if needed.
     *
     * Private method that gets called when calling requestShutdown().
     * This method cleanly shutsdown each system.
     *
     */
    void processShutdown();

    /** @brief Tracks whether the game loop is active. */
    bool running;

    /** @brief Immutable configuration set at construction. Const ensures runtime modifications don't destabilize systems. */
    const ApplicationSpecifications specifications;

    /** @brief Counts frames for profiling and debugging. Useful for frame-rate independent calculations. */
    int frameCounter;

    /** @brief Duration of the last frame in seconds. Drives delta-time calculations for smooth, frame-rate independent updates. */
    float frameTime;

    /** @brief Timestamp of the previous frame. Required to calculate frameTime each iteration. */
    float lastFrameTime;

    /** @brief Clamped frame time used for physics and gameplay. Prevents spiral of death when frame rate drops. */
    float timeStep;

    /** @brief Polymorphic renderer handle. Unique_ptr ensures single ownership and automatic cleanup. */
    std::unique_ptr<IRender> renderer;
};
