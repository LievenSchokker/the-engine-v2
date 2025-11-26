#include "Networking/Serialization/CerealWriteArchive.h"

CerealWriteArchive::CerealWriteArchive()
: stream(std::ios::binary)
, archive(stream)
{}

CerealWriteArchive::~CerealWriteArchive() = default;

std::vector<std::byte> CerealWriteArchive::getBytes() const
{
    auto data = stream.str();
    return {reinterpret_cast<const std::byte*>(data.data()),
          reinterpret_cast<const std::byte*>(data.data() + data.size())};
}