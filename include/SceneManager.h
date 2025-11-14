#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Scene.h"

/**
 * @brief Coordinates ownership and activation of scenes.
 *
 * Stores scenes by name, forwards lifecycle calls, and keeps track of the
 * active scene along with a simple paused state.
 */
class SceneManager {
   public:
    SceneManager() = default;

    /**
     * @brief Register a scene owned by the manager.
     *
     * If a scene with the same name already exists, nullptr is returned to
     * indicate an error and the scene is not added.
     *
     * @param scene Scene instance that transfers ownership to the manager.
     * @return Pointer to the stored scene, or nullptr when the argument is
     * null or a scene with the same name already exists.
     */
    Scene* addScene(std::unique_ptr<Scene> scene);

    /**
     * @brief Remove a stored scene.
     *
     * When the removed scene is the active one it receives @ref Scene::onStop
     * and the active pointer is cleared.
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
     * @brief Get the currently active scene.
     *
     * @return Pointer to the active scene, or nullptr when none is active.
     */
    Scene* getActiveScene() const {
        return activeScene;
    }

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
    bool isPaused() const {
        return paused;
    }

    /**
     * @brief Update the active scene when not paused.
     *
     * @param deltaTime Seconds elapsed since the previous update call.
     */
    void update(float deltaTime);

    /**
     * @brief Render the active scene when not paused.
     */
    void render();

   private:
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* activeScene = nullptr;
    bool paused = false;
};
