#pragma once

#include "Core/ApplicationSpecifications.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

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
	[[nodiscard]] ApplicationSpecifications getApplicationSpecifications()
		const;

	/**
	 * @brief Configures how the engine should initialize the application.
	 *
	 * @param specifications Desired application configuration.
	 */
	void setApplicationSpecifications(
		const ApplicationSpecifications& specifications);

	/**
	 * @brief Registers a scene with the engine.
	 * If no active scene is registered the set as active scene.
	 *
	 * @param scene Scene to transfer ownership of.
	 */
	void addScene(std::unique_ptr<Scene> scene);

	/**
	 * @brief Activate a new scene.
	 *
	 * @param name Name of the scene to activate.
	 * @return true when the scene exists and becomes active, false otherwise.
	 */
	bool setActiveScene(const std::string& name) const;

	std::unique_ptr<SceneManager> getSceneManager();

   private:
	ApplicationSpecifications specifications;
	std::unique_ptr<SceneManager> sceneManager;
};