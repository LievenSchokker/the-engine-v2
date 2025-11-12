#pragma once

#include "GameObject.h"

#include <memory>
#include <string>
#include <vector>


/**
 * @brief Collection of game objects that can be started, updated, and rendered.
 *
 * Maintains lifecycle state and propagates core calls to its game objects when
 * active.
 */
class Scene
{
  public:
    /**
     * @brief Create a scene with the given identifier.
     *
     * @param name Human-readable name used for lookups in the scene manager.
     */
    explicit Scene(std::string name);

    /**
     * @brief Retrieve the scene name.
     *
     * @return Reference to the stored scene name.
     */
    const std::string& getName() const
    {
        return name;
    }

    /**
     * @brief Add a game object to the scene.
     *
     * If the scene is currently active the object receives @ref
     * GameObject::onStart.
     *
     * @param gameObject Game object instance to own.
     * @return Pointer to the stored game object, or nullptr if the argument is
     * null.
     */
    GameObject* addGameObject(std::unique_ptr<GameObject> gameObject);

    /**
     * @brief Remove a game object by name.
     *
     * When the scene is active the object receives @ref GameObject::onStop
     * before removal.
     *
     * @param name Name of the game object to remove.
     * @return true when an object was removed, false otherwise.
     */
    bool removeGameObject(const std::string& name);

    /**
     * @brief Look up a game object by name.
     *
     * @param name Name of the game object to retrieve.
     * @return Pointer to the object, or nullptr when not found.
     */
    GameObject* getGameObject(const std::string& name) const;

    /**
     * @brief Start the scene if it is not already active.
     *
     * Triggers @ref GameObject::onStart for every stored object.
     */
    void onStart();

    /**
     * @brief Stop the scene if it is active.
     *
     * Triggers @ref GameObject::onStop for every stored object.
     */
    void onStop();

    /**
     * @brief Update all game objects when the scene is active.
     *
     * @param deltaTime Seconds elapsed since the previous update.
     */
    void update(float deltaTime);

    /**
     * @brief Render all game objects when the scene is active.
     */
    void render();

  private:
    std::string name;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    bool active = false;
};
