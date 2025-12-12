#pragma once


#include <functional>
#include <memory>
#include <cstdint>

class GameObject;

/**
 * @brief Registry mapping asset IDs to GameObject factory functions.
 *
 * Both server and client register the same prefabs so spawning
 * creates identical objects on both sides.
 */
class NetworkPrefabRegistry
{
public:
	using Factory = std::function<std::unique_ptr<GameObject>()>;

	static NetworkPrefabRegistry& instance();

	void registerPrefab(uint32_t assetId, Factory factory);
	std::unique_ptr<GameObject> create(uint32_t assetId) const;
	bool hasAsset(uint32_t assetId) const;

private:
	NetworkPrefabRegistry() = default;
	std::unordered_map<uint32_t, Factory> prefabs;
};