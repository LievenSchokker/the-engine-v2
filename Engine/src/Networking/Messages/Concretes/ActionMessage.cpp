#include "Networking/Messages/ConcreteMessages/ActionMessage.h"
#include "Networking/Serialization/Serialization.h"

ActionMessage::ActionMessage() : networkComponentIdentity(0)
                                 , networkGameObjectIdentity(0)
                                 , actionKey("")
                                 , tick(0)
{
}

ActionMessage::ActionMessage(uint32_t componentId, uint32_t objectId,
                             std::string action, uint32_t tickRate)
	: networkComponentIdentity(componentId)
	  , networkGameObjectIdentity(objectId)
	  , actionKey(std::move(action))
	  , tick(tickRate)
{
}

std::vector<std::byte> ActionMessage::serialize() const
{
	WriteArchive archive;

	archive.process(const_cast<uint32_t&>(networkComponentIdentity));
	archive.process(const_cast<uint32_t&>(networkGameObjectIdentity));
	archive.process(const_cast<std::string&>(actionKey));
	archive.process(const_cast<uint32_t&>(tick));

	// Serialize payload
	uint32_t payloadSize = static_cast<uint32_t>(payload.size());
	archive.process(payloadSize);
	if (payloadSize > 0)
	{
		archive.processBytes(payload.data(), payloadSize);
	}

	return archive.getBytes();
}

bool ActionMessage::deserialize(const std::byte* data, size_t length)
{
	if (!data || length == 0) return false;

	try
	{
		ReadArchive archive(data, length);

		archive.process(networkComponentIdentity);
		archive.process(networkGameObjectIdentity);
		archive.process(actionKey);
		archive.process(tick);

		// Deserialize payload
		uint32_t payloadSize;
		archive.process(payloadSize);
		payload.resize(payloadSize);
		if (payloadSize > 0)
		{
			archive.processBytes(payload.data(), payloadSize);
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool ActionMessage::validate() const
{
	return !actionKey.empty() && !(
		       networkComponentIdentity == 0 && networkGameObjectIdentity == 0);
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

const std::vector<std::byte>& ActionMessage::getPayload() const
{
	return payload;
}

void ActionMessage::setPayload(std::vector<std::byte> data)
{
	payload = std::move(data);
}

MessageTypes ActionMessage::getMessageType() const
{
	return MessageTypes::ActionMessage;
}