//
// Created by thijs on 17-11-2025.
//


#include "engine/networking/RawMessage.h"
#include "engine/networking/SendMode.h"
#include <cstdint>
#include <string>


// Construct from raw bytes
RawMessage::RawMessage(int cid, const std::byte* data, size_t len, SendMode sendMode)
    : connectionID(cid), payload(data, data + len), sendMode(sendMode), length(len)
{
}

// Construct from std::string
RawMessage::RawMessage(int cid, const std::string& str, SendMode sendMode)
    : connectionID(cid), payload(reinterpret_cast<const std::byte*>(str.data()),
                                 reinterpret_cast<const std::byte*>(str.data()) + str.size()),
      sendMode(sendMode), length(str.size())
{
}

// Convert payload to string
std::string RawMessage::toString() const
{
    return std::string(reinterpret_cast<const char*>(payload.data()), payload.size());
}

int RawMessage::getConnectionID() const
{
    return connectionID;
}

std::vector<std::byte> RawMessage::getPayload() const
{
    return payload;
}

SendMode RawMessage::getSendMode() const
{
    return sendMode;
}

uint16_t RawMessage::getMessageType() const
{
    return messageType;
}

size_t RawMessage::getLength() const
{
    return length;
}

void RawMessage::setConnectionID(int cid)
{
    connectionID = cid;
}

void RawMessage::setPayload(const std::byte* data, size_t len)
{
    payload.assign(data, data + len);
    length = payload.size();
}

void RawMessage::setPayload(const std::vector<std::byte>& newPayload)
{
    payload = newPayload;
    length = payload.size();
}

void RawMessage::setPayload(const std::string& str)
{
    payload.resize(str.size());
    std::memcpy(payload.data(), str.data(), str.size());
    length = payload.size();
}

void RawMessage::setSendMode(SendMode mode)
{
    sendMode = mode;
}

void RawMessage::setMessageType(uint16_t type)
{
    messageType = type;
}
