#pragma once


#include "NetworkBehaviour.h"
#include "Networking/Component/ComponentFactory.h"
#include "Core/GameWorld.h"

template <typename... Args>
void NetworkBehaviour::callCommand(const std::string& name, Args&&... args)
{
	if (!isClient() || !identity)
	{
		return;
	}

	if (!gameWorld)
	{
		return;
	}

	std::vector<std::byte> payload;
	if constexpr (sizeof...(Args) > 0)
	{
		WriteArchive archive;
		(archive.process(const_cast<std::remove_cvref_t<Args>&>(args)), ...);
		payload = archive.getBytes();
	}

	ActionMessage message(
		componentNetworkId,
		identity->getNetId(),
		name,
		0
		);

	message.setPayload(std::move(payload));

	gameWorld->sendToServer(message);
}

inline void NetworkBehaviour::addCommand(const std::string& name,
                                         ActionCallback callback)
{
	commands[name] = std::move(callback);
}

inline AuthorityType NetworkBehaviour::getAuthorityType() const
{
	return authorityType;
}
