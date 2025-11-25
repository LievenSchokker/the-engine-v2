#include "Networking/Serialization/CerealArchiveAdapter.h"

#include <memory>
#include <vector>

class IArchive;

std::unique_ptr<IArchive> createWriteArchive()
{
	return std::make_unique<CerealWriteArchive>();
}

std::unique_ptr<IArchive> createReadArchive(const uint8_t* data, size_t length)
{
	return std::make_unique<CerealReadArchive>(data, length);
}

std::vector<uint8_t> getArchiveBytes(IArchive* archive)
{
	auto* cerealArchive = static_cast<CerealWriteArchive*>(archive);
	return cerealArchive->getBytes();
}