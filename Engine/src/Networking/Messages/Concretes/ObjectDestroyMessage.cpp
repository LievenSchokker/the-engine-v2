#include "Networking/Messages/Concretes/ObjectDestroyMessage.h"
#include "Networking/Serialization/Serialization.h"

std::vector<std::byte> ObjectDestroyMessage::serialize() const
{
    WriteArchive archive;
    archive.process(netId);
    return archive.getBytes();
}

bool ObjectDestroyMessage::deserialize(const std::byte* data, size_t length)
{
    if (!data || length == 0) return false;

    try
    {
        ReadArchive archive(data, length);
        archive.process(netId);
        return validate();
    }
    catch (...)
    {
        return false;
    }
}

bool ObjectDestroyMessage::validate() const
{
    return netId > 0;
}