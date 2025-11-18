#pragma once


#include "ConnectionStatus.h"
#include "core/Networking/NetworkID.h"
#include "c"

struct Connection {
    NetworkId networkId;
    int transportConnectionId;
    ConnectionStatus connectionStatus;
};
