#pragma once

#include <functional>
#include <unordered_map>
#include <memory>

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

    static NetworkPrefabRegistry& instance()
    {
        static NetworkPrefabRegistry registry;
        return registry;
    }

    void registerPrefab(uint32_t assetId, Factory factory)
    {
        prefabs[assetId] = std::move(factory);
    }

    std::unique_ptr<GameObject> create(const uint32_t assetId) const
    {
        auto it = prefabs.find(assetId);
        if (it != prefabs.end())
        {
            return it->second();
        }
        return nullptr;
    }

    bool hasAsset(const uint32_t assetId) const
    {
        return prefabs.contains(assetId);
    }

private:
    NetworkPrefabRegistry() = default;
    std::unordered_map<uint32_t, Factory> prefabs;
};