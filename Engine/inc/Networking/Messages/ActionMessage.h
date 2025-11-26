#pragma once


#include <string>
#include <vector>
#include <cstdint>

#include "Networking/Serialization/Serialization.h"

class ActionMessage
{
public:
	ActionMessage() = default;
	ActionMessage(uint32_t compId, uint32_t objId, std::string action, uint32_t tick);

    std::vector<std::byte> serialize() const;
	bool deserialize(const std::byte* data, size_t length);

	bool validate() const;

	uint32_t getComponentIdentity() const;
	uint32_t getGameObjectIdentity() const;
	const std::string& getAction() const;
	uint32_t getTick() const;

	void setComponentIdentity(uint32_t id);
	void setGameObjectIdentity(uint32_t id);
	void setAction(std::string action);
	void setTick(uint32_t tick);

private:
	uint32_t networkComponentIdentity;
	uint32_t networkGameObjectIdentity;
	std::string actionKey;
	uint32_t tick;
};
