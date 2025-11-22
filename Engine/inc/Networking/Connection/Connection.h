#pragma once


#include "ConnectionStatus.h"

struct Connection {
    ConnectionMode mode;
    int transportConnectionId;
    ConnectionStatus connectionStatus;
};
