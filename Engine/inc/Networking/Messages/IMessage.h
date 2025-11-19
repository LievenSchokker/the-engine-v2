#pragma once
#include <cstddef>
#include <vector>


class IMessage
{
public:
    virtual ~IMessage() = default;

    virtual std::vector<std::byte> serialize() const = 0;
    virtual bool deserialize(const std::byte* data, size_t length) = 0;
    virtual bool validate() const = 0;
};
