#pragma once


#include "Core/GameWorld.h"
#include "AI/Navigation/NavigationGridOptions.h"

class NavigationSystem;
class GameObject;
class Behaviour;
struct RenderQueue;
struct ShapeRenderCommand;

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Core/GameWorld.h"

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
	~Scene();

	/**
	 * @brief Retrieve the scene name.
	 *
	 * @return Reference to the stored scene name.
	 */
	const std::string& getName() const;
    int addGameObject(std::unique_ptr<GameObject> gameObject);

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
     * @brief Removes a GameObject from the scene (destroys it).
     */
    void removeGameObject(GameObject* obj);

    /**
     * @brief Look up a game object by name.
     *
     * @param name Name of the game object to retrieve.
     * @return Pointer to the object, or nullptr when not found.
     */
    GameObject *getGameObject(const std::string &name) const;

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
	void onStart(GameWorld& world);

	/**
	 * @brief Stop the scene if it is active.
	 *
	 * Deactivates every stored object's components through their component
	 * manager.
	 */
	void onStop();
	void onPause();

	/**
	 * Destroys all GameObjects in this scene and clears the @c gameObjects vector.
	 *
	 * Calls GameObject::onSceneDestroy() on each GameObject inside @c gameObjects, then clears the entire vector to remove the GameObjects from memory.
	 */
	void destroyAllGameObjects();

	template <class T>
	std::vector<T*> getAllComponentsOfType() const;

	/**
 * @brief Retrieves the id stored by this scene for a given GameObject.
 * @param gameObject the gameObject to look with for its id
 * @return the id that is assigned to the GameObject if found, -1 if the GameObject does not belong to this scene
 */
	int getSceneId(const GameObject& gameObject) const;

	/**
	 * @brief Returns a GameObject from  this scene by providing its scene id.
	 *
	 * Returns
	 * @param id
	 * @return the GameObject whose id matches the argument, nullptr if the id is not found on any of this scene's GameObjects.
	 */
	GameObject* getGameObjectById(int id) const;


	NavigationSystem* getNavigationSystem() const;

	/**
     ** @brief Removes a GameObject by pointer and returns it.
	*/
    std::unique_ptr<GameObject> extractGameObject(GameObject* obj);

    /**
     * @brief Gets mutable access to all GameObjects.
     */
    std::vector<std::unique_ptr<GameObject>>& getGameObjects();

    const std::vector<std::unique_ptr<GameObject>>& getGameObjects() const;


    NavigationSystem* getNavigationSystem();
private:
	std::string name;
	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<GameObject*> destroyQueue;
	bool active = false;


	void initialiseNavigationSystem(NavigationGridOptions options);
    std::unique_ptr<NavigationSystem> navigationSystem;

    /// Incremented everytime a GameObject is added to this scene.
	std::vector<Behaviour*> beforeEnableBehaviours;
    GameWorld* gameWorld = nullptr;

};

#include "Scene.inl"