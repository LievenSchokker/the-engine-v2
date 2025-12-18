#include "Networking/Messages/ConcreteMessages/SpawnMessage.h"
#include "Networking/Serialization/Serialization.h"

std::vector<std::byte> SpawnMessage::serialize() const
{
	WriteArchive archive;

	// Network identity
	archive.process(netId);
	archive.process(assetId);
	archive.process(ownerId);

	//We need a bool to pass into the archive it cannot take in true or false directly
	if (bool hasGameObject = (gameObject != nullptr))
	{
		archive.process(hasGameObject);
		gameObject->serialize(archive);
	}
	else
	{
		archive.process(hasGameObject);
	}

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

        bool hasGameObject = false;
        archive.process(hasGameObject);

        if (hasGameObject)
        {
            gameObject = std::make_unique<GameObject>();
            gameObject->deserialize(archive);
        }
        return validate();
    } catch (const std::exception& e) {
        return false;
    } catch (...) {
        return false;
    }
}

bool SpawnMessage::validate() const
{
	return netId > 0 && assetId > 0;
}