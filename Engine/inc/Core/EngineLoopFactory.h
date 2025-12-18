#pragma once

#include <memory>

class IEngineLoop;
class Game;

/**
 * @class EngineLoopFactory
 * @brief Factory class for creating configured EngineLoop instances.
 *
 * This factory encapsulates the complex initialization logic required to create
 * an EngineLoop with all necessary subsystems based on the application specifications
 * defined in the Game instance.
 *
 *
 * @see ApplicationSpecifications
 *
 */
class EngineLoopFactory
{
public:
    /**
     * @brief Creates a fully configured EngineLoop based on game specifications.
     *
     * Reads the ApplicationSpecifications from the provided Game instance and
     * initializes the appropriate engine systems based on the EngineSystem flags.
     *
     * @param game Unique pointer to the Game instance. Ownership is transferred
     *             to the created EngineLoop.
     *
     * @return Unique pointer to the configured IEngineLoop instance.
     *
     * @note The Game must have valid ApplicationSpecifications set before calling
     *       this method, including the desired EngineSystem flags.
     */
    static std::unique_ptr<IEngineLoop> createEngineLoop(std::unique_ptr<Game> game);
};