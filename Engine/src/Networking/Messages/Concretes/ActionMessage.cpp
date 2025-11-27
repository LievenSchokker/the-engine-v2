#include "Networking/Messages/Concretes/ActionMessage.h"
#include "Networking/Serialization/Serialization.h"

ActionMessage::ActionMessage()   : networkComponentIdentity(0)
      , networkGameObjectIdentity(0)
      , actionKey("")
      , tick(0)
{

}

ActionMessage::ActionMessage(uint32_t componentId, uint32_t objectId, std::string action, uint32_t tickRate)
    : networkComponentIdentity(componentId)
      , networkGameObjectIdentity(objectId)
      , actionKey(std::move(action))
      , tick(tickRate)
{
}

std::vector<std::byte> ActionMessage::serialize() const
{
    WriteArchive archive;
    archive.process(networkComponentIdentity);
    archive.process(networkGameObjectIdentity);
    archive.process(actionKey);
    archive.process(tick);
    uint32_t payloadSize = static_cast<uint32_t>(payload.size());
    archive.process(payloadSize);

    auto bytes = archive.getBytes();
    bytes.insert(bytes.end(), payload.begin(), payload.end());

    return archive.getBytes();
}

bool ActionMessage::deserialize(const std::byte* data, const size_t length)
{
    if (!data || length == 0) return false;

    try
    {
        ReadArchive archive(data, length);
        archive.process(networkComponentIdentity);
        archive.process(networkGameObjectIdentity);
        archive.process(actionKey);
        archive.process(tick);

        uint32_t payloadSize;
        archive.process(payloadSize);

        // Read remaining bytes as payload
        payload.clear();
        if (payloadSize > 0)
        {
            size_t bytesRead = archive.getBytesRead();
            if (bytesRead + payloadSize <= length)
            {
                payload.assign(data + bytesRead, data + bytesRead + payloadSize);
            }
        }

        return validate();
    }
    catch (...)
    {
        return false;
    }
}

const std::byte* ActionMessage::getPayloadData() const
{
    return payload.empty() ? nullptr : payload.data();
}

size_t ActionMessage::getPayloadSize() const
{
    return payload.size();
}

void ActionMessage::setPayload(std::vector<std::byte> data)
{
    payload = std::move(data);
}

bool ActionMessage::validate() const
{
    return !actionKey.empty() && !(networkComponentIdentity == 0 && networkGameObjectIdentity == 0);
}

uint32_t ActionMessage::getComponentIdentity() const
{
    return networkComponentIdentity;
}

uint32_t ActionMessage::getGameObjectIdentity() const
{
    return networkGameObjectIdentity;
}

const std::string& ActionMessage::getAction() const
{
    return actionKey;
}

uint32_t ActionMessage::getTick() const
{
    return tick;
}

void ActionMessage::setComponentIdentity(const uint32_t id)
{
    networkComponentIdentity = id;
}

void ActionMessage::setGameObjectIdentity(const uint32_t id)
{
    networkGameObjectIdentity = id;
}

void ActionMessage::setAction(std::string action)
{
    actionKey = std::move(action);
}

void ActionMessage::setTick(uint32_t t)
{
    tick = t;
}

MessageTypes ActionMessage::getMessageType() const
{
    return MessageTypes::ActionMessage;
}