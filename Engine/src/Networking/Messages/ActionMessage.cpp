#include "Networking/Messages/ActionMessage.h"
#include "Networking/Serialization/Serialization.h"

ActionMessage::ActionMessage(uint32_t compId, uint32_t objId, std::string action, uint32_t t)
	: networkComponentIdentity(compId)
	, networkGameObjectIdentity(objId)
	, actionKey(std::move(action))
	, tick(t)
{}

void ActionMessage::process(IArchive& archive)
{
	archive.process(networkComponentIdentity);
	archive.process(networkGameObjectIdentity);
	archive.process(actionKey);
	archive.process(tick);
}

std::vector<uint8_t> ActionMessage::serialize() const
{
	auto archive = createWriteArchive();
	const_cast<ActionMessage*>(this)->process(*archive);
	return getArchiveBytes(archive.get());
}

bool ActionMessage::deserialize(const uint8_t* data, size_t length)
{
	if (!data || length == 0) return false;

	try
	{
		auto archive = createReadArchive(data, length);
		process(*archive);
		return validate();
	}
	catch (...)
	{
		return false;
	}
}

bool ActionMessage::validate() const
{
	return !actionKey.empty() &&
		   !(networkComponentIdentity == 0 && networkGameObjectIdentity == 0);
}

uint32_t ActionMessage::getComponentIdentity() const { return networkComponentIdentity; }
uint32_t ActionMessage::getGameObjectIdentity() const { return networkGameObjectIdentity; }
const std::string& ActionMessage::getAction() const { return actionKey; }
uint32_t ActionMessage::getTick() const { return tick; }

void ActionMessage::setComponentIdentity(uint32_t id) { networkComponentIdentity = id; }
void ActionMessage::setGameObjectIdentity(uint32_t id) { networkGameObjectIdentity = id; }
void ActionMessage::setAction(std::string action) { actionKey = std::move(action); }
void ActionMessage::setTick(uint32_t t) { tick = t; }