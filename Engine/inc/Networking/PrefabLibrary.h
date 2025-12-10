#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class GameObject;

/**
 * @brief Container for prefab templates.
 *
 * Prefabs are template GameObjects that can be instantiated at runtime.
 * Objects with NetworkBehaviour are automatically registered for network spawning.
 */
class PrefabLibrary
{
public:
    PrefabLibrary() = default;
    ~PrefabLibrary() = default;

    /**
     * @brief Adds a prefab to the library with auto-assigned assetId.
     *
     * @param prefab The prefab template
     * @return The assigned assetId (0 if prefab was null)
     */
    uint32_t add(std::unique_ptr<GameObject> prefab);

    /**
     * @brief Adds a prefab with a specific assetId.
     *
     * @param assetId The specific ID to assign
     * @param prefab The prefab template
     */
    void add(uint32_t assetId, std::unique_ptr<GameObject> prefab);

    /**
     * @brief Creates an instance (clone) of a prefab by assetId.
     *
     * @param assetId The prefab ID
     * @return A new GameObject clone, or nullptr if not found
     */
    std::unique_ptr<GameObject> instantiate(uint32_t assetId) const;

    /**
     * @brief Creates an instance (clone) of a prefab by name.
     *
     * @param name The prefab name
     * @return A new GameObject clone, or nullptr if not found
     */
    std::unique_ptr<GameObject> instantiate(const std::string& name) const;

    /**
     * @brief Gets the template (not a clone) - for inspection only.
     */
    GameObject* getTemplate(uint32_t assetId) const;
    GameObject* getTemplate(const std::string& name) const;

    /**
     * @brief Gets assetId by prefab name.
     *
     * @return assetId, or 0 if not found
     */
    uint32_t getAssetId(const std::string& name) const;

    /**
     * @brief Gets all network-enabled prefab assetIds.
     */
    const std::vector<uint32_t>& getNetworkPrefabIds() const;

    /**
     * @brief Checks if a prefab exists.
     */
    bool contains(uint32_t assetId) const;
    bool contains(const std::string& name) const;

    /**
     * @brief Gets the total number of registered prefabs.
     */
    size_t size() const;

private:
    uint32_t nextAssetId = 1;
    std::unordered_map<uint32_t, std::unique_ptr<GameObject>> prefabsById;
    std::unordered_map<std::string, uint32_t> nameToId;
    std::vector<uint32_t> networkPrefabIds;
};