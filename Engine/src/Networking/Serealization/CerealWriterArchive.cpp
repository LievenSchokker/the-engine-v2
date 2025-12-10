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
    auto data = stream.str();
    auto span = std::as_bytes(std::span{data});
    return {span.begin(), span.end()};
}