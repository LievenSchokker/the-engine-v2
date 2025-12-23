#pragma once


#include <vector>
#include <functional>

class GameObject;
class Scene;

/**
 * @brief Handles deferred destruction of GameObjects.
 *
 * Objects are queued during gameplay and destroyed at a safe point
 * (end of frame) to avoid iterator invalidation and use-after-free bugs.
 */
class DestroySystem
{
public:
    DestroySystem() = default;
    ~DestroySystem() = default;

    DestroySystem(const DestroySystem&) = delete;
    DestroySystem& operator=(const DestroySystem&) = delete;

    /**
     * @brief Queue a GameObject for destruction at end of frame.
     * @param obj The GameObject to destroy. Null objects are ignored.
     */
    void queueDestroy(GameObject* obj);

    /**
     * @brief Queue multiple GameObjects for destruction.
     * @param objects Vector of GameObjects to destroy.
     */
    void queueDestroy(const std::vector<GameObject*>& objects);

    /**
     * @brief Process all queued objects and remove them from the scene.
     *
     * Calls onSceneDestroy() on each object, then removes it from the scene.
     * Clears the queue after processing.
     *
     * @param scene The scene to remove objects from.
     */
    void processQueue(Scene& scene);

    /**
     * @brief Check if a GameObject is queued for destruction.
     * @param obj The GameObject to check.
     * @return True if the object is in the destruction queue.
     */
    [[nodiscard]] bool isQueued(const GameObject* obj) const;

    /**
     * @brief Get the number of objects queued for destruction.
     * @return Number of queued objects.
     */
    [[nodiscard]] size_t queueSize() const;

    /**
     * @brief Clear the destruction queue without processing.
     *
     * Use with caution - objects will not be destroyed.
     * Useful when shutting down a scene entirely.
     */
    void clear();

    /**
     * @brief Set a callback to be invoked before each object is destroyed.
     *
     * Useful for cleanup, logging, or notifying other systems.
     *
     * @param callback Function called with the GameObject about to be destroyed.
     */
    void setPreDestroyCallback(std::function<void(GameObject*)> callback);

private:
    std::vector<GameObject*> destroyQueue;
    std::function<void(GameObject*)> preDestroyCallback;
};