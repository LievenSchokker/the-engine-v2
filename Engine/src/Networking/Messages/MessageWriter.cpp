#pragma once


#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/SendMode.h"


#include <cstring>


OutgoingRawMessage MessageWriter::writeMessage(
    const IMessage& message,
    int connectionId,
    SendMode sendMode)
{
    return {connectionId, message.serialize(), sendMode};
}
