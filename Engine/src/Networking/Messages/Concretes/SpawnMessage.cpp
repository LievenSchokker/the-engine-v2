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

        std::cout << "[SpawnMessage] netId=" << netId << " assetId=" << assetId << " ownerId=" << ownerId << std::endl;

        bool hasGameObject = false;
        archive.process(hasGameObject);
        std::cout << "[SpawnMessage] hasGameObject=" << hasGameObject << std::endl;

        if (hasGameObject)
        {
            gameObject = std::make_unique<GameObject>();
            gameObject->deserialize(archive);

            std::cout << "[SpawnMessage] GameObject created: " << gameObject->getName() << std::endl;
            std::cout << "[SpawnMessage] Components: " << gameObject->getComponentCount() << std::endl;
            std::cout << "[SpawnMessage] Behaviours: " << gameObject->getAllBehaviours().size() << std::endl;
        }
        return validate();
    } catch (const std::exception& e) {
        std::cerr << "[SpawnMessage] Exception: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "[SpawnMessage] Unknown exception" << std::endl;
        return false;
    }
}

bool SpawnMessage::validate() const
{
	return netId > 0 && assetId > 0;
}