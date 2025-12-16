#pragma once

#include "GameObject/GameObject.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"
#include <cstdint>
#include <vector>

#include "Math/Vector2.h"

/**
 * @brief Message sent when a networked object is spawned.
 *
 * Contains all information clients need to instantiate a local
 * representation of a server-spawned object.
 */
struct SpawnMessage final : public IMessage
{
public:
    SpawnMessage() = default;

	SpawnMessage(const SpawnMessage&) = delete;
	SpawnMessage& operator=(const SpawnMessage&) = delete;
	SpawnMessage(SpawnMessage&&) = default;
	SpawnMessage& operator=(SpawnMessage&&) = default;

    MessageTypes getMessageType() const override
    {
        return MessageTypes::SpawnMessage;
    }

    std::vector<std::byte> serialize() const override;
    bool deserialize(const std::byte* data, size_t length) override;
    bool validate() const override;

    uint32_t netId = 0;           ///< Server-assigned network identifier
    uint32_t assetId = 0;         ///< Prefab/asset type for instantiation
    int ownerId = -1;             ///< Owning client (-1 = server)
    std::unique_ptr<GameObject> gameObject;
};