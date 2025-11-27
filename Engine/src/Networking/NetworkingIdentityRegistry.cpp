#include "Networking/NetworkingIdentityRegistry.h"

#include <iostream>

#include "Networking/NetworkIdentity.h"
#include "Scene/Scene.h"

NetworkIdentityRegistry::NetworkIdentityRegistry()
    : currentScene(nullptr)
{
}

NetworkIdentityRegistry::~NetworkIdentityRegistry()
{
    clear();
}

void NetworkIdentityRegistry::setCurrentScene(Scene* scene)
{
    currentScene = scene;
}

void NetworkIdentityRegistry::registerIdentity(NetworkIdentity* identity)
{
    if (!identity)
    {
        std::cerr << "[Registry] registerIdentity called with null\n";
        return;
    }

    uint32_t netId = identity->getNetId();
    std::cout << "[Registry] Registering identity netId=" << netId << std::endl;

    if (netId == 0)
    {
        std::cerr << "[Registry] WARNING: Registering identity with netId=0!\n";
        // Print stack trace or add breakpoint here
        return;
    }

    identitiesByNetId[netId] = identity;
    addToOwnerIndex(identity);

    std::cout << "[Registry] SUCCESS: Now contains " << identitiesByNetId.size() << " identities\n";
}

void NetworkIdentityRegistry::unregisterIdentity(NetworkIdentity* identity)
{
    if (!identity)
    {
        return;
    }

    uint32_t netId = identity->getNetId();

    auto it = identitiesByNetId.find(netId);
    if (it != identitiesByNetId.end())
    {
        removeFromOwnerIndex(identity);
        identitiesByNetId.erase(it);
    }
}

NetworkIdentity* NetworkIdentityRegistry::findByNetId(uint32_t netId) const
{
    std::cout << "[Registry] Looking for netId=" << netId
              << ", registry has " << identitiesByNetId.size() << " entries\n";

    auto it = identitiesByNetId.find(netId);
    if (it != identitiesByNetId.end())
    {
        std::cout << "[Registry] Found!\n";
        return it->second;
    }

    std::cout << "[Registry] Not found. Registered netIds: ";
    for (const auto& [id, ptr] : identitiesByNetId)
    {
        std::cout << id << " ";
    }
    std::cout << "\n";

    return nullptr;
}

std::vector<NetworkIdentity*> NetworkIdentityRegistry::getAllIdentities() const
{
    std::vector<NetworkIdentity*> result;
    result.reserve(identitiesByNetId.size());

    for (const auto& pair : identitiesByNetId)
    {
        result.push_back(pair.second);
    }

    return result;
}

std::vector<NetworkIdentity*> NetworkIdentityRegistry::getIdentitiesByOwner(int ownerId) const
{
    auto it = identitiesByOwner.find(ownerId);
    if (it != identitiesByOwner.end())
    {
        return it->second; // Returns copy
    }
    return {};
}

void NetworkIdentityRegistry::clear()
{
    identitiesByNetId.clear();
    identitiesByOwner.clear();
}

void NetworkIdentityRegistry::forEach(const std::function<void(NetworkIdentity*)>& callback) const
{
    for (const auto& pair : identitiesByNetId)
    {
        callback(pair.second);
    }
}

void NetworkIdentityRegistry::addToOwnerIndex(NetworkIdentity* identity)
{
    int ownerId = identity->getOwnerId();
    identitiesByOwner[ownerId].push_back(identity);
}

void NetworkIdentityRegistry::removeFromOwnerIndex(NetworkIdentity* identity)
{
    int ownerId = identity->getOwnerId();

    auto ownerIt = identitiesByOwner.find(ownerId);
    if (ownerIt != identitiesByOwner.end())
    {
        auto& vec = ownerIt->second;
        vec.erase(
            std::remove(vec.begin(), vec.end(), identity),
            vec.end()
        );

        // Clean up empty vectors
        if (vec.empty())
        {
            identitiesByOwner.erase(ownerIt);
        }
    }
}