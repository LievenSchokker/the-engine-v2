#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

class GameObject;

/**
 * @brief Stores prefab templates and instantiates clones.
 */
class PrefabLibrary
{
public:
	PrefabLibrary() = default;
	~PrefabLibrary() = default;

	/**
	 * @brief Adds a prefab with auto-assigned assetId.
	 * @return The assigned assetId (0 if prefab was null)
	 */
	uint32_t add(std::unique_ptr<GameObject> prefab);

	/**
	 * @brief Adds a prefab with a specific assetId.
	 */
	void add(uint32_t assetId, std::unique_ptr<GameObject> prefab);

	/**
	 * @brief Creates a clone of a prefab by assetId.
	 */
	std::unique_ptr<GameObject> instantiate(uint32_t assetId) const;

	/**
	 * @brief Creates a clone of a prefab by name.
	 */
	std::unique_ptr<GameObject> instantiate(const std::string& name) const;

	/**
	 * @brief Gets the template (not a clone) - for inspection only.
	 */
	GameObject* getTemplate(uint32_t assetId) const;
	GameObject* getTemplate(const std::string& name) const;

	/**
	 * @brief Gets assetId by prefab name.
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

	/**
	 * @brief Gets the total number of registered prefabs.
	 */
	size_t size() const;

private:
	uint32_t nextAssetId = 1;
	std::unordered_map<uint32_t, std::unique_ptr<GameObject>> prefabsById;
	std::vector<uint32_t> networkPrefabIds;
};