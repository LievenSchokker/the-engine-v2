#include "Networking/Serialization/CerealWriteArchive.h"

#include <span>

CerealWriteArchive::CerealWriteArchive()
	: stream(std::ios::binary)
	  , archive(stream)
{
}

CerealWriteArchive::~CerealWriteArchive() = default;

std::vector<std::byte> CerealWriteArchive::getBytes() const
{
<<<<<<< HEAD
	std::string data = stream.str();

	std::vector<std::byte> result;
	result.reserve(data.size());
	for (char c : data) {
		result.push_back(static_cast<std::byte>(c));
	}
	return result;
=======
	auto data = stream.str();
	auto span = std::as_bytes(std::span{data});
	return {span.begin(), span.end()};
>>>>>>> origin/development
}