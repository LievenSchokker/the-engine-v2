#pragma once

#include "Core/ApplicationSpecifications.h"
#include "Scene/Scene.h"

/**
 * @class Game
 * @brief Entry point for game developers to integrate with the engine.
 *
 * This class serves as the primary injection point where game developers
 * configure their application and register scenes.
 * 
 */
class Game
{
public:
    Game();
    ~Game();

    /**
     * @brief Retrieves a copy of the current application specifications.
     *
     * @return Copy of the application specifications.
     */
    [[nodiscard]] ApplicationSpecifications getApplicationSpecifications() const;

    /**
     * @brief Configures how the engine should initialize the application.
     *
     * @param specifications Desired application configuration.
     */
    void setApplicationSpecifications(const ApplicationSpecifications& specifications);

    /**
     * @brief Registers a scene with the engine.
     *
     * @param scene Scene to transfer ownership of.
     */
    void addScene(std::unique_ptr<Scene> scene);

private:
    ApplicationSpecifications specifications;

    /// Owning container for all registered scenes. Using unique_ptr ensures
    /// deterministic destruction order during engine shutdown.
    std::vector<std::unique_ptr<Scene>> scenes;
};