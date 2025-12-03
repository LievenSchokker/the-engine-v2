#pragma once

#include "../Rendering/Color.h"
#include "../Rendering/RenderQueue/RenderQueue.h"
#include "Scene.h"

#include <memory>
#include <string>
#include <unordered_map>

/**
 * @brief Coordinates ownership and activation of scenes.
 *
 * Stores scenes by name, forwards lifecycle calls, and keeps track of the
 * active scene along with a simple paused state.
 */
class SceneManager
{
   public:
	SceneManager() = default;

	/**
	 * @brief Register a scene owned by the manager.
	 *
	 * @param scene Scene instance that transfers ownership to the manager.
	 * @return true if the scene was successfully added, false if scene was null
	 * or a scene with the same name already exists.
	 */
	bool addScene(std::unique_ptr<Scene> scene);

	/**
	 * @brief Remove a stored scene.
	 *
	 * When the removed scene is the active one it receives @ref Scene::onStop
	 * and the active pointer is cleared.
	 *
	 * @warning Removing a scene destroys all of its owned game objects.
	 * Scenes own their game objects via std::unique_ptr; erasing the scene
	 * will delete those objects. If you need to preserve objects, extract or
	 * transfer them before removal using @ref Scene::extractGameObject or
	 * @ref transferGameObject.
	 *
	 * @param name Name of the scene to remove.
	 * @return true when the scene existed and was removed, false otherwise.
	 */
	bool removeScene(const std::string& name);

	/**
	 * @brief Look up a scene by name.
	 *
	 * @param name Name of the scene.
	 * @return Pointer to the scene, or nullptr when not found.
	 */
	Scene* getScene(const std::string& name) const;

	/**
	 * @brief Transfer a game object from one scene to another.
	 *
	 * Extracts the game object from the source scene and adds it to the target
	 * scene. Component activation/deactivation is handled automatically based
	 * on the active state of the source and destination scenes.
	 *
	 * @param fromSceneName Name of the source scene.
	 * @param toSceneName Name of the target scene.
	 * @param objectName Name of the game object to transfer.
	 * @return true when the transfer succeeds, false otherwise (scene not
	 * found, object not found, or object already exists in target scene).
	 */
	bool transferGameObject(const std::string& fromSceneName,
							const std::string& toSceneName,
							const std::string& objectName) const;

	/**
	 * @brief Get the currently active scene.
	 *
	 * @return Pointer to the active scene, or nullptr when none is active.
	 */
	Scene* getActiveScene() const;

	/**
	 * @brief Activate a new scene.
	 *
	 * Stops the previous scene if necessary, clears the paused state, and
	 * starts the requested scene.
	 *
	 * @param name Name of the scene to activate.
	 * @return true when the scene exists and becomes active, false otherwise.
	 */
	bool setActiveScene(const std::string& name);

	/**
	 * @brief Alias for @ref setActiveScene.
	 *
	 * @param name Name of the scene to activate.
	 * @return true when the scene exists and becomes active, false otherwise.
	 */
	bool loadScene(const std::string& name);

	/**
	 * @brief Pause the active scene.
	 *
	 * Invokes @ref Scene::onPause and suppresses update/render calls until
	 * @ref resume is invoked.
	 */
	void pause();

	/**
	 * @brief Resume a paused scene.
	 *
	 * Invokes @ref Scene::onResume and re-enables update/render calls.
	 */
	void resume();

	/**
	 * @brief Query the paused state.
	 *
	 * @return true when the active scene is paused.
	 */
	bool isPaused() const;

	/**
	 * @brief Update the active scene when not paused.
	 *
	 * @param deltaTime Seconds elapsed since the previous update call.
	 */
	void update(float deltaTime);

	/**
	 * @brief Change the clear color used at the start of each frame.
	 */
	void setClearColor(const Color& color);

	/**
	 * @brief Retrieve the currently configured clear color.
	 */
	Color getClearColor() const;

   private:
	std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
	Scene* activeScene = nullptr;
	bool paused = false;
	IRenderer* renderer = nullptr;
	Color clearColor = Color::black();
};
