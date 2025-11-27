#include "Networking/Serialization/CerealWriteArchive.h"

CerealWriteArchive::CerealWriteArchive()
	: stream(std::ios::binary)
	  , archive(stream)
{
}

CerealWriteArchive::~CerealWriteArchive() = default;

std::vector<std::byte> CerealWriteArchive::getBytes() const
{
	std::string data = stream.str();

	std::vector<std::byte> result;
	result.reserve(data.size());
	for (char c : data) {
		result.push_back(static_cast<std::byte>(c));
	}
	return result;
}