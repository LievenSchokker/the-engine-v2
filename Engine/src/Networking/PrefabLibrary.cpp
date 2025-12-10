#include "Networking/PrefabLibrary.h"
#include "GameObject/GameObject.h"
#include "Networking/NetworkIdentity.h"
#include "Networking/NetworkBehaviour.h"

#include <iostream>

uint32_t PrefabLibrary::add(std::unique_ptr<GameObject> prefab)
{
    if (!prefab)
    {
        std::cerr << "[PrefabLibrary] Cannot add null prefab" << std::endl;
        return 0;
    }

    uint32_t assetId = nextAssetId++;

    // Check if this prefab has any NetworkBehaviours
    bool hasNetworkBehaviour = false;
    for (const auto& component : prefab->getComponents())
    {
        if (dynamic_cast<NetworkBehaviour*>(component.get()))
        {
            hasNetworkBehaviour = true;
            break;
        }
    }

    // Auto-add NetworkIdentity if it has NetworkBehaviours but no identity
    auto* identity = prefab->getComponent<NetworkIdentity>();
    if (hasNetworkBehaviour && !identity)
    {
        identity = prefab->addComponent<NetworkIdentity>();
    }

    // Configure NetworkIdentity with assetId
    if (identity)
    {
        identity->setAssetId(assetId);
        networkPrefabIds.push_back(assetId);
    }

    // Store name mapping
    std::string name = prefab->getName();
    if (!name.empty())
    {
        nameToId[name] = assetId;
    }

    std::cout << "[PrefabLibrary] Added prefab '" << name
              << "' with assetId=" << assetId
              << (hasNetworkBehaviour ? " (networked)" : "") << std::endl;

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

    // Check for NetworkBehaviours
    bool hasNetworkBehaviour = false;
    for (const auto& component : prefab->getComponents())
    {
        if (dynamic_cast<NetworkBehaviour*>(component.get()))
        {
            hasNetworkBehaviour = true;
            break;
        }
    }

    auto* identity = prefab->getComponent<NetworkIdentity>();
    if (hasNetworkBehaviour && !identity)
    {
        identity = prefab->addComponent<NetworkIdentity>();
    }

    if (identity)
    {
        identity->setAssetId(assetId);
        networkPrefabIds.push_back(assetId);
    }

    std::string name = prefab->getName();
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
    return prefabsById.contains(assetId);
}

bool PrefabLibrary::contains(const std::string& name) const
{
    return nameToId.contains(name);
}

size_t PrefabLibrary::size() const
{
    return prefabsById.size();
}