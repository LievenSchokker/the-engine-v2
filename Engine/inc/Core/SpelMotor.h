#pragma once

#include "ApplicationSpecifications.h"


class Timer;
class IRender;
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

    ~SpelMotor();

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

    /** @brief Tracks whether the game loop is active. */
    bool running;

    /** @brief Immutable configuration set at construction.
     * Const ensures runtime modifications don't destabilize systems. */
    const ApplicationSpecifications specifications;

    /** @brief renderer handle. */
    std::unique_ptr<IRender> renderer;

    /** @brief timeStep calculation for engine */
    std::unique_ptr<Timer> timer;
};
