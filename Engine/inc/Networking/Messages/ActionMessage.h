#pragma once


#include <string>
#include <vector>
#include <cstdint>

class ActionMessage
{
public:
	ActionMessage() = default;
	ActionMessage(uint32_t compId, uint32_t objId, std::string action, uint32_t tick);

	std::vector<uint8_t> serialize() const;
	bool deserialize(const uint8_t* data, size_t length);

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
	template<typename Archive>
	void process(Archive& archive)
	{
		archive.process(networkComponentIdentity);
		archive.process(networkGameObjectIdentity);
		archive.process(actionKey);
		archive.process(tick);
	}

	uint32_t networkComponentIdentity{0};
	uint32_t networkGameObjectIdentity{0};
	std::string actionKey;
	uint32_t tick{0};
};