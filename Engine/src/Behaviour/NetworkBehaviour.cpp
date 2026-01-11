#include "Behaviour/NetworkBehaviour.h"

#include "Core/GameWorld.h"

NetworkBehaviour::NetworkBehaviour() = default;

bool NetworkBehaviour::isServer() const
{
	return gameWorld && gameWorld->isServer();
}

bool NetworkBehaviour::isClient() const
{
	return gameWorld && gameWorld->isClient();
}

bool NetworkBehaviour::hasAuthority() const
{
	if ( !identity ) return false;
	return identity->hasAuthority();
}

void NetworkBehaviour::executeAction(const std::string& actionKey,
									 const std::vector<std::byte>& payload)
{
	auto it = commands.find(actionKey);
	if ( it == commands.end() ) return;

	ReadArchive archive(payload.data(), payload.size());
	it->second(archive);
}

void NetworkBehaviour::setComponentNetworkId(uint32_t id)
{
	componentNetworkId = id;
}

void NetworkBehaviour::serialize(WriteArchive &archive) const
{
	archive.process(componentNetworkId);
	archive.process(authorityType);
}

void NetworkBehaviour::deserialize(ReadArchive &archive)
{
	archive.process(componentNetworkId);
	archive.process(authorityType);
}
