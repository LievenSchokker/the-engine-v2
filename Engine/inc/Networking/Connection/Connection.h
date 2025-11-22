#pragma once


#include "ConnectionStatus.h"
#include "ConnectionMode.h"

struct Connection {
    ConnectionMode mode;
    int transportConnectionId;
    ConnectionStatus connectionStatus;
};
