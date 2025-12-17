#pragma once

#include "Scene.h"
#include "Core/IEngineSystems.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Component/NetworkIdentity.h"
#include "Networking/PrefabLibrary.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Rendering/Color.h"

/**
 * @brief Coordinates ownership and activation of scenes.
 *
 * Stores scenes by name, forwards lifecycle calls, and keeps track of the
 * active scene along with a simple paused state.
 */
class SceneManager : public IEngineSystem
{
   public:
	SceneManager();

    /**
     * @brief Configures the SceneManager for networking.
     * Must be called before setActiveScene() for network processing to work.
     *
     * @param mode Server or Client mode
     * @param spawnManager Required for server to auto-spawn objects (can be nullptr for client)
     */
    void configureNetworking(ConnectionMode mode, NetworkSpawnManager* spawnManager = nullptr);

    /**
     * @brief Checks if network processing has been configured.
     */
    bool isNetworkConfigured() const;
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
	 * @param gameWorld
	 */
	void update(double deltaTime, const GameWorld& gameWorld) override;

	/**
	 * @brief Get or create the persistent scene.
	 *
	 * The persistent scene is always active and never stopped, making it ideal
	 * for debug controls and other cross-scene utilities. It persists across
	 * all scene transitions.
	 *
	 * @return Pointer to the persistent scene
	 */
	Color getClearColor() const;
	void applyNetworkSnapshot(
		const std::vector<std::unique_ptr<GameObject>>& receivedObjects) const;
    bool isLocallyOwned(NetworkIdentity* identity) const;

    void setWorld(GameWorld* world) { gameWorld = world; }
	Scene* getOrCreatePersistentScene();

	/**
	 * @brief Get the persistent scene.
	 *
	 * @return Pointer to the persistent scene, or nullptr if not created yet.
	 */
	Scene* getPersistentScene() const;


    std::string getFirstSceneName() const;
    
   private:

    /**
 * @brief Processes a scene for networking based on configured mode.
 */
    void processSceneForNetwork(Scene& scene);

    /**
     * @brief Server: Extract NetworkBehaviour objects as prefabs.
     */
    void processForServer(Scene& scene);

    /**
     * @brief Client: Remove NetworkBehaviour objects.
     */
    void processForClient(Scene& scene);

    /**
     * @brief Checks if a GameObject has any NetworkBehaviour components.
     */
    bool hasNetworkBehaviour(const GameObject& obj) const;

    /**
     * @brief Checks if a GameObject has a NetworkIdentity.
     */
    bool hasNetworkIdentity(const GameObject& obj) const;

    bool networkConfigured = false;
    ConnectionMode networkMode = ConnectionMode::Client;
    NetworkSpawnManager* spawnManager = nullptr;
    std::unordered_set<std::string> processedScenes;

    GameWorld* gameWorld = nullptr;
	void setScenes(std::vector<std::unique_ptr<Scene>> scene);

	SystemStatus start(GameWorld& gameWorld) override;

	const std::string getName() const override;
	std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
	Scene* activeScene = nullptr;
	std::unique_ptr<Scene> persistentScene; // Always-active scene for debug/utilities
	bool paused = false;
};