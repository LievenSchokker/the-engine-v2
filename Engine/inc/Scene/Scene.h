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
    bool addGameObject(std::unique_ptr<GameObject> gameObject);

    /**
	 * @brief Add a game object to the scene.
	 *
	 * If the scene is currently active the object's components are activated
	 * via its @ref ComponentManager.
	 *
	 * @param gameObject Game object instance to own.
	 * @param world
	 * @return true if the object was successfully added, false if gameObject
	 * was null.
	 */
	bool addRunTimeGameObject(std::unique_ptr<GameObject> gameObject, GameWorld& world);

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
     * @param world
     */
    void update(double deltaTime, const GameWorld& world);

    /**
	* @brief initialises the @c behaviours by calling their awake(), onEnable() and start() methods in the correct order.
	* Does not enable the behaviours on their own only calls the callback.
	* @param behaviours the behaviours that need to be initialised.
	* @param world
     */
    void initialiseBehaviours(const std::vector<Behaviour*>& behaviours, GameWorld& world);

	/**
	 * @brief Adds the provided GameObject to the @c destroyQueue vector, in order to delete and destroy the object when @c processDestroyQueue is called.
	 * @param gameObject the GameObject to destroy
	 */
	void queueDestroy(GameObject* gameObject);

    /**
     * @brief processes the destroy queue by destroying and deleting all GameObjects inside it,
     * This function calls @c GameObject::onSceneDestroy() for each GameObject inside the @c destroyQueue,
     * then attempts to remove the GameObject from the stored @c gameObjects vector to delete it, then clears the @c destroyQueue vector to begin the next frame clean.
     *
     * This function is called at the end of each scene::update() call.
     */
    void processDestroyQueue();

	/**
	 * Checks whether the given object is in the @c destroyQueue vector in order to be destroyed.
	 * @param gameObject GameObject to check
	 * @return true if the GameObject is in the vector, false otherwise.
	 */
	bool isInDestroyQueue(GameObject* gameObject);

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



private:
	/// @brief Method that removes the @c gameObject from the containers storing it
	bool removeGameObjectInternal(GameObject* gameObject);

	/// @brief Method that adds the @c gameObject to the internal containers storing it
	bool addGameObjectInternal(std::unique_ptr<GameObject> gameObject);


	std::string name;
	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<GameObject*> destroyQueue;
	bool active = false;

	/// Map stores the GameObject and their scene id.
	std::map<const GameObject*, int> gameObjectIds;
	int currentGameObjectId = 0;
	void initialiseNavigationSystem(NavigationGridOptions options);
	std::unique_ptr<NavigationSystem> navigationSystem;

    /// Incremented everytime a GameObject is added to this scene.
	std::vector<Behaviour*> beforeEnableBehaviours;
    GameWorld* gameWorld = nullptr;

};

#include "Scene.inl"