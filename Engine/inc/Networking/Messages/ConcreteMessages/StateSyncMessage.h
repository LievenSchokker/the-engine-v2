#pragma once


#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"
#include "GameObject/GameObject.h"

#include <vector>
#include <cstdint>
#include <memory>

/**
 * @brief Synchronizes full GameObject state for networked objects.
 */
class StateSyncMessage final : public IMessage
{
public:
	StateSyncMessage() = default;
	~StateSyncMessage() override = default;

	StateSyncMessage(const StateSyncMessage&) = delete;
	StateSyncMessage& operator=(const StateSyncMessage&) = delete;
	StateSyncMessage(StateSyncMessage&&) = default;
	StateSyncMessage& operator=(StateSyncMessage&&) = default;

	MessageTypes getMessageType() const override;
	std::vector<std::byte> serialize() const override;
	bool deserialize(const std::byte* data, size_t length) override;
	bool validate() const override;

	void addGameObject(std::unique_ptr<GameObject> obj);
	void clear();

	uint32_t tick = 0;
	std::string currentSceneName;
	std::vector<std::unique_ptr<GameObject>> gameObjects;
};