#pragma once

#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"
#include "Networking/Serialization/Serialization.h"
#include <vector>
#include <cstdint>

/**
 * @brief Synchronizes transform state for multiple objects in a single message.
 *
 * Batches all dirty object states into one packet per tick to minimize
 * network overhead. Only contains transform data - component state uses
 * separate mechanisms.
 */
class StateSyncMessage final : public IMessage
{
public:
    struct ObjectState
    {
        uint32_t netId;
        float posX;
        float posY;
        float rotation;
    };

    StateSyncMessage() = default;

    MessageTypes getMessageType() const override
    {
        return MessageTypes::StateSyncMessage;
    }

    std::vector<std::byte> serialize() const override
    {
        WriteArchive archive;

        archive.process(const_cast<uint32_t&>(tick));

        uint32_t count = static_cast<uint32_t>(objects.size());
        archive.process(count);

        for (const auto& obj : objects)
        {
            archive.process(const_cast<uint32_t&>(obj.netId));
            archive.process(const_cast<float&>(obj.posX));
            archive.process(const_cast<float&>(obj.posY));
            archive.process(const_cast<float&>(obj.rotation));
        }

        return archive.getBytes();
    }

    bool deserialize(const std::byte* data, size_t length) override
    {
        if (!data || length == 0) return false;

        try
        {
            ReadArchive archive(data, length);

            archive.process(tick);

            uint32_t count = 0;
            archive.process(count);

            objects.clear();
            objects.reserve(count);

            for (uint32_t i = 0; i < count; ++i)
            {
                ObjectState obj{};
                archive.process(obj.netId);
                archive.process(obj.posX);
                archive.process(obj.posY);
                archive.process(obj.rotation);
                objects.push_back(obj);
            }

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool validate() const override
    {
        return true;
    }

    void addObject(uint32_t netId, float x, float y, float rot)
    {
    	std::cout<< "Adding object: " << netId << ", x: " << x << ", y: "
    	<< y << std::endl;
        objects.push_back({netId, x, y, rot});
    }

    void clear()
    {
        objects.clear();
        tick = 0;
    }

    uint32_t tick = 0;
    std::vector<ObjectState> objects;
};