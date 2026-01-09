#include "Networking/PrefabLibrary.h"

#include "Component/NetworkIdentity.h"
#include "GameObject/GameObject.h"

#include <iostream>

#include "Behaviour/NetworkBehaviour.h"

uint32_t PrefabLibrary::add(std::unique_ptr<GameObject> prefab)
{
    if (prefab == nullptr) return 0;

    const uint32_t assetId = nextAssetId++;

	if (prefab->getParent() != nullptr && !prefab->getParent()->hasComponent<NetworkIdentity>())
	{
		std::cerr << "[Network] Networked object cannot have non-networked parent\n";
		return 0;
	}

    if (prefab->getComponent<NetworkIdentity>())
    {
        networkPrefabIds.push_back(assetId);
    }

    prefabsById[assetId] = std::move(prefab);
    return assetId;
}

void PrefabLibrary::add(const uint32_t assetId, std::unique_ptr<GameObject> prefab)
{
    if (!prefab)
    {
        return;
    }

	if (prefab->getParent() != nullptr && !prefab->getParent()->hasComponent<NetworkIdentity>())
	{
		std::cerr << "[Network] Networked object cannot have non-networked parent\n";
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


    prefabsById[assetId] = std::move(prefab);
}

std::unique_ptr<GameObject> PrefabLibrary::instantiate(const uint32_t assetId) const
{
	const auto it = prefabsById.find(assetId);
	if (it == prefabsById.end())
	{
		return nullptr;
	}


	if (!it->second)
	{
		return nullptr;
	}
	return it->second->clone();
}

GameObject* PrefabLibrary::getTemplate(uint32_t assetId) const
{
    auto it = prefabsById.find(assetId);
    return (it != prefabsById.end()) ? it->second.get() : nullptr;
}

const std::vector<uint32_t>& PrefabLibrary::getNetworkPrefabIds() const
{
    return networkPrefabIds;
}

bool PrefabLibrary::contains(const uint32_t assetId) const
{
    return prefabsById.contains(assetId);
}

size_t PrefabLibrary::size() const
{
    return prefabsById.size();
}

std::vector<uint32_t> PrefabLibrary::getNetworkPrefabIdsWithAuthoritativeClient() const
{
	std::vector<uint32_t> result;

	for (const auto& [id, gameObject] : prefabsById)
	{
		auto behaviours = gameObject->getComponents<NetworkBehaviour>();

		for (const auto* behaviour : behaviours)
		{
			if (behaviour->getAuthorityType() == SpawnOnType::ClientSpawn)
			{
				result.push_back(id);
				break;
			}
		}
	}

	return result;
}