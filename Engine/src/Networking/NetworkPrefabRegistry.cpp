#include "Networking/NetworkPrefabRegistry.h"
#include "GameObject/GameObject.h"

NetworkPrefabRegistry& NetworkPrefabRegistry::instance()
{
	static NetworkPrefabRegistry registry;
	return registry;
}

void NetworkPrefabRegistry::registerPrefab(const uint32_t assetId, Factory factory)
{
	prefabs[assetId] = std::move(factory);
}

std::unique_ptr<GameObject> NetworkPrefabRegistry::create(const uint32_t assetId) const
{
	auto prefabEntry = prefabs.find(assetId);
	if (prefabEntry != prefabs.end())
	{
		return prefabEntry->second();
	}
	return nullptr;
}

bool NetworkPrefabRegistry::hasAsset(const uint32_t assetId) const
{
	return prefabs.contains(assetId);
}