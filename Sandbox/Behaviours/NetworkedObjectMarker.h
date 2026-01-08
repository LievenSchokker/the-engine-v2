#pragma once

#include "Behaviour/NetworkBehaviour.h"
#include "Networking/Serialization/RegistrationBase.h"

/**
 * @brief Minimal NetworkBehaviour to mark server-authoritative objects for sync.
 */
class NetworkedObjectMarker final : public NetworkBehaviour,
                                    public RegistrationBase<NetworkedObjectMarker>
{
public:
	static constexpr const char* name()
	{
		return "NetworkedObjectMarker";
	}

	const char* getName() const override
	{
		return name();
	}
};
