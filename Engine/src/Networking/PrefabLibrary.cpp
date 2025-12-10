#include "Networking/PrefabLibrary.h"

#include "Component/NetworkIdentity.h"
#include "GameObject/GameObject.h"

#include <iostream>

uint32_t PrefabLibrary::add(std::unique_ptr<GameObject> prefab)
{
    if (!prefab)
    {
        std::cerr << "[PrefabLibrary] Cannot add null prefab" << std::endl;
        return 0;
    }

    uint32_t assetId = nextAssetId++;
    std::string name = prefab->getName();

    // Check if has NetworkIdentity (is a network prefab)
    if (prefab->getComponent<NetworkIdentity>())
    {
        networkPrefabIds.push_back(assetId);
    }

    // Store name mapping
    if (!name.empty())
    {
        nameToId[name] = assetId;
    }

    std::cout << "[PrefabLibrary] Added prefab '" << name
              << "' with assetId=" << assetId << std::endl;

    prefabsById[assetId] = std::move(prefab);
    return assetId;
}

void PrefabLibrary::add(uint32_t assetId, std::unique_ptr<GameObject> prefab)
{
    if (!prefab)
    {
        std::cerr << "[PrefabLibrary] Cannot add null prefab" << std::endl;
        return;
    }

    // Update nextAssetId to avoid conflicts
    if (assetId >= nextAssetId)
    {
        nextAssetId = assetId + 1;
    }

    std::string name = prefab->getName();

    if (prefab->getComponent<NetworkIdentity>())
    {
        networkPrefabIds.push_back(assetId);
    }

    if (!name.empty())
    {
        nameToId[name] = assetId;
    }

    std::cout << "[PrefabLibrary] Added prefab '" << name
              << "' with assetId=" << assetId << std::endl;

    prefabsById[assetId] = std::move(prefab);
}

std::unique_ptr<GameObject> PrefabLibrary::instantiate(uint32_t assetId) const
{
    auto it = prefabsById.find(assetId);
    if (it == prefabsById.end())
    {
        std::cerr << "[PrefabLibrary] Prefab not found: assetId=" << assetId << std::endl;
        return nullptr;
    }

    return it->second->clone();
}

std::unique_ptr<GameObject> PrefabLibrary::instantiate(const std::string& name) const
{
    auto it = nameToId.find(name);
    if (it == nameToId.end())
    {
        std::cerr << "[PrefabLibrary] Prefab not found: name=" << name << std::endl;
        return nullptr;
    }

    return instantiate(it->second);
}

GameObject* PrefabLibrary::getTemplate(uint32_t assetId) const
{
    auto it = prefabsById.find(assetId);
    return (it != prefabsById.end()) ? it->second.get() : nullptr;
}

GameObject* PrefabLibrary::getTemplate(const std::string& name) const
{
    auto it = nameToId.find(name);
    if (it == nameToId.end()) return nullptr;
    return getTemplate(it->second);
}

uint32_t PrefabLibrary::getAssetId(const std::string& name) const
{
    auto it = nameToId.find(name);
    return (it != nameToId.end()) ? it->second : 0;
}

const std::vector<uint32_t>& PrefabLibrary::getNetworkPrefabIds() const
{
    return networkPrefabIds;
}

bool PrefabLibrary::contains(uint32_t assetId) const
{
    return prefabsById.find(assetId) != prefabsById.end();
}

bool PrefabLibrary::contains(const std::string& name) const
{
    return nameToId.find(name) != nameToId.end();
}

size_t PrefabLibrary::size() const
{
    return prefabsById.size();
}