#include "CerealSerializer.h"
#include "Networking/Serialization/CerealSerializer.h"

#include <stdexcept>

std::vector<uint8_t> CerealSerializer::serializeImpl(const void* object, size_t typeHash)
{

	throw std::runtime_error("Use templated serialize() method");
}

void CerealSerializer::deserializeImpl(const std::vector<uint8_t>& data, void* object, size_t typeHash)
{
	throw std::runtime_error("Use templated deserialize() method");
}