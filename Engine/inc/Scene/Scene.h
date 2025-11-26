#pragma once

#include "GameObject/GameObject.h"
#include "Behaviour/Behaviour.h"
#include <memory>
#include <string>
#include <vector>

struct ShapeRenderCommand;

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
	const std::string& getName() const;

	/**
	 * @brief Add a game object to the scene.
	 *
	 * If the scene is currently active the object's components are activated
	 * via its @ref ComponentManager.
	 *
	 * @param gameObject Game object instance to own.
	 * @return true if the object was successfully added, false if gameObject
	 * was null.
	 */
	bool addGameObject(std::unique_ptr<GameObject> gameObject);

	/**
	 * @brief Remove a game object by name.
	 *
	 * When the scene is active the object's components are deactivated via its
	 * @ref ComponentManager before removal.
	 *
	 * @warning This call destroys the game object since the scene owns it via
	 * std::unique_ptr. If you need to keep the object alive and move it
	 * elsewhere, use @ref extractGameObject instead and add it to another
	 * scene.
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
	 * @brief Extract a game object from the scene without destroying it.
	 *
	 * Removes the game object from the scene and transfers ownership to the
	 * caller. When the scene is currently active, the object's components are
	 * deactivated via its @ref ComponentManager before extraction.
	 *
	 * @param name Name of the game object to extract.
	 * @return unique_ptr to the extracted game object, or nullptr if not found.
	 */
	std::unique_ptr<GameObject> extractGameObject(const std::string& name);

	/**
	 * @brief Start the scene if it is not already active.
	 *
	 * Activates every stored object's components through their component
	 * manager.
	 */
	void onStart();

	/**
	 * @brief Stop the scene if it is active.
	 *
	 * Deactivates every stored object's components through their component
	 * manager.
	 */
	void onStop();

	/**
	 * @brief Pause the scene if it is active.
	 *
	 * Temporarily suspends component execution for every stored object.
	 */
	void onPause();

	/**
	 * @brief Resume the scene if it is active.
	 *
	 * Resumes component execution for every stored object.
	 */
	void onResume();

	/**
	 * @brief Update all game objects when the scene is active.
	 *
	 * @param deltaTime Seconds elapsed since the previous update.
	 */
	void update(float deltaTime) const;

	/**
	 * @brief Collect render commands for active objects in this scene.
	 *
	 * Appends commands to the provided collection; does nothing when inactive.
	 */
	void collectRenderCommands(std::vector<ShapeRenderCommand>& out) const;

    void initialiseBehaviours(const std::vector<Behaviour*>& behaviours);
   private:
	std::string name;
	std::vector<std::unique_ptr<GameObject>> gameObjects;
	bool active = false;
};
