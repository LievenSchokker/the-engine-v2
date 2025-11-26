#include "Networking/NetworkingIdentityRegistry.h"
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
        return;
    }

    uint32_t netId = identity->getNetId();

    // Prevent duplicate registration
    if (identitiesByNetId.find(netId) != identitiesByNetId.end())
    {
        // Already registered - could log warning here
        return;
    }

    identitiesByNetId[netId] = identity;
    addToOwnerIndex(identity);
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
    auto it = identitiesByNetId.find(netId);
    if (it != identitiesByNetId.end())
    {
        return it->second;
    }
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