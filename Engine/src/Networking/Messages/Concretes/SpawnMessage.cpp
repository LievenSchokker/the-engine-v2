#include "Networking/Messages/Concretes/SpawnMessage.h"
#include "Networking/Serialization/Serialization.h"

std::vector<std::byte> SpawnMessage::serialize() const
{
	WriteArchive archive;
	archive.process(netId);
	archive.process(assetId);
	archive.process(ownerId);
	archive.process(sceneId);
	archive.process(position.x);
	archive.process(position.y);
	archive.process(rotation);
	archive.process(scale.x);
	archive.process(scale.y);

	uint32_t payloadSize = static_cast<uint32_t>(payload.size());
	archive.process(payloadSize);

	return archive.getBytes();
}

bool SpawnMessage::deserialize(const std::byte* data, size_t length)
{
	if (!data || length == 0) return false;

	try {
		ReadArchive archive(data, length);
		archive.process(netId);
		archive.process(assetId);
		archive.process(ownerId);
		archive.process(sceneId);
		archive.process(position.x);
		archive.process(position.y);
		archive.process(rotation);
		archive.process(scale.x);
		archive.process(scale.y);

		uint32_t payloadSize;
		archive.process(payloadSize);

		return validate();
	} catch (...) {
		return false;
	}
}

bool SpawnMessage::validate() const
{
	return netId > 0 && assetId > 0;
}