#pragma once

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
    uint64_t sceneId = 0;         ///< Target scene (for multi-scene support)
    Vector2 position{0, 0};       ///< Initial world position
    double rotation = 0;       ///< Initial rotation
    Vector2 scale{1, 1};          ///< Initial scale
    std::vector<std::byte> payload; ///< Serialized initial state
};