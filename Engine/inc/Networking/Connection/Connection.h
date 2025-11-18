#pragma once


#include "ConnectionStatus.h"

struct Connection {
    int transportConnectionId;
    ConnectionStatus connectionStatus;
};
