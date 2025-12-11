#include "Networking/NetworkingIdentityRegistry.h"

#include <iostream>

#include "../../inc/Component/NetworkIdentity.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

NetworkIdentityRegistry::NetworkIdentityRegistry()
	: gameWorld(nullptr)
{
}

NetworkIdentityRegistry::~NetworkIdentityRegistry()
{
	clear();
}

void NetworkIdentityRegistry::setGameWorld(GameWorld* gameWorld)
{
	this->gameWorld = gameWorld;
}

Scene* NetworkIdentityRegistry::getCurrentScene() const
{
	if (gameWorld != nullptr)
	{
		return gameWorld->sceneManager->getActiveScene();
	}

	return nullptr;
}

void NetworkIdentityRegistry::registerIdentity(NetworkIdentity* identity)
{
	if (!identity) {
		return;
	}

	const uint32_t netId = identity->getNetId();

	if (netId == 0) {
		return;
	}

	identitiesByNetId[netId] = identity;
	addToOwnerIndex(identity);
}

void NetworkIdentityRegistry::unregisterIdentity(NetworkIdentity* identity)
{
	if (!identity) {
		return;
	}

	const uint32_t netId = identity->getNetId();

	if (auto it = identitiesByNetId.find(netId); it != identitiesByNetId.end()) {
		removeFromOwnerIndex(identity);
		identitiesByNetId.erase(it);
	}
}

NetworkIdentity* NetworkIdentityRegistry::findByNetId(uint32_t netId) const
{
	auto it = identitiesByNetId.find(netId);
	if (it != identitiesByNetId.end()) {
		return it->second;
	}
	return nullptr;
}

std::vector<NetworkIdentity*> NetworkIdentityRegistry::getAllIdentities() const
{
	std::vector<NetworkIdentity*> result;
	result.reserve(identitiesByNetId.size());

	for (const auto& pair : identitiesByNetId) {
		result.push_back(pair.second);
	}

	return result;
}

std::vector<NetworkIdentity*> NetworkIdentityRegistry::getIdentitiesByOwner(
	int ownerId) const
{
	auto it = identitiesByOwner.find(ownerId);
	if (it != identitiesByOwner.end()) {
		return it->second;
	}
	return {};
}

void NetworkIdentityRegistry::clear()
{
	identitiesByNetId.clear();
	identitiesByOwner.clear();
}

void NetworkIdentityRegistry::forEach(
	const std::function<void(NetworkIdentity*)>& callback) const
{
	for (const auto& pair : identitiesByNetId) {
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
	if (ownerIt != identitiesByOwner.end()) {
		auto& networkIdentitys = ownerIt->second;
		networkIdentitys.erase(
			std::ranges::remove(networkIdentitys,
			                    identity).begin(),
			networkIdentitys.end()
			);

		if (networkIdentitys.empty()) {
			identitiesByOwner.erase(ownerIt);
		}
	}
}