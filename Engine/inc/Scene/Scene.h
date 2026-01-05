#pragma once


#include "Core/GameWorld.h"
#include "AI/Navigation/NavigationGridOptions.h"
#include "SlotMap/SlotMap.h"

class GameObject;
class NavigationSystem;

class Behaviour;
struct RenderQueue;
struct ShapeRenderCommand;
struct ObjectHandle;

#include "Core/GameWorld.h"


#include <map>
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
	~Scene();

	/**
	 * @brief Retrieve the scene name.
	 *
	 * @return Reference to the stored scene name.
	 */
	[[nodiscard]] const std::string& getName() const;

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

    ObjectHandle addGameObject(std::unique_ptr<GameObject> gameObject);

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
    bool removeGameObject(ObjectHandle handle);

    /**
     * @brief Look up a game object by name.
     *
     * @param name Name of the game object to retrieve.
     * @return Pointer to the object, or nullptr when not found.
     */
	GameObject* getGameObject(const std::string& name);
	GameObject* getGameObject(ObjectHandle handle);
    [[nodiscard]] const GameObject* getGameObject(const std::string& name) const;
    [[nodiscard]] const GameObject* getGameObject(ObjectHandle handle) const;

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
    std::unique_ptr<GameObject> extractGameObject(ObjectHandle handle);

	/**
	 * Destroys all GameObjects in this scene and clears the @c gameObjects vector.
	 *
	 * Calls GameObject::onSceneDestroy() on each GameObject inside @c gameObjects, then clears the entire vector to remove the GameObjects from memory.
	 */
	void destroyAllGameObjects();

    NavigationSystem* getNavigationSystem();

    template <class Component>
    std::vector<Component*> getAllComponentsOfType() const;

    template <class Func>
    void forEachGameObject(Func&& func);

    template <class Func>
    void forEachGameObject(Func&& func) const;

	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;



private:
	std::string name;
	SlotMap<GameObject> gameObjects;

	bool active = false;
	GameWorld* gameWorld = nullptr;

	//TODO THESE SEEM SMELLY REMOVE
	std::unique_ptr<NavigationSystem> navigationSystem;

    [[nodiscard]] ObjectHandle findHandleByName(const std::string& name) const;
    [[nodiscard]] bool isValid(ObjectHandle handle) const;
    void initialiseNavigationSystem(NavigationGridOptions options);
};

#include "Scene.inl"