#include "Networking/Messages/ConcreteMessages/StateSyncMessage.h"
#include "Networking/Serialization/Serialization.h"
#include "GameObject/GameObject.h"

MessageTypes StateSyncMessage::getMessageType() const
{
	return MessageTypes::StateSyncMessage;
}

std::vector<std::byte> StateSyncMessage::serialize() const
{
	WriteArchive archive;

	archive.process(const_cast<uint32_t&>(tick));
	archive.process(const_cast<std::string&>(currentSceneName));

	uint32_t count = static_cast<uint32_t>(gameObjects.size());
	archive.process(count);

	for (const auto& gameObject : gameObjects)
	{
		if (gameObject)
		{
			gameObject->serialize(archive);
		}
	}

	return archive.getBytes();
}

bool StateSyncMessage::deserialize(const std::byte* data, size_t length)
{
	if (!data || length == 0) return false;

	try
	{
		ReadArchive archive(data, length);

		archive.process(tick);
		archive.process(currentSceneName);

		uint32_t count = 0;
		archive.process(count);

		gameObjects.clear();
		gameObjects.reserve(count);

		for (uint32_t i = 0; i < count; ++i)
		{
			auto gameObject = std::make_unique<GameObject>();
			gameObject->deserialize(archive);
			gameObjects.push_back(std::move(gameObject));
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool StateSyncMessage::validate() const
{
	if (currentSceneName.empty())
	{
		return false;
	}

	if (currentSceneName.length() > 256)
	{
		return false;
	}

	constexpr size_t MAX_OBJECTS = 10000;

	if (gameObjects.size() > MAX_OBJECTS)
	{
		return false;
	}

	return true;
}

void StateSyncMessage::addGameObject(std::unique_ptr<GameObject> obj)
{
	gameObjects.push_back(std::move(obj));
}

void StateSyncMessage::clear()
{
	gameObjects.clear();
	tick = 0;
	currentSceneName.clear();
}