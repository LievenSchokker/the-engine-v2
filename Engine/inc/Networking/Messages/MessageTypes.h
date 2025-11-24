#pragma once


#include <cstdint>


enum class MessageTypes : uint8_t
{
    ConnectionMessage = 0,
    ClientStatusChangesMessage = 1,
    CommandMessage = 2,
    RpcMessage = 3,
    NetworkDebugMessage = 4,
    TimeSnapshotMessage = 5,
    SwitchSceneMessage = 6,
    SpawnGameObjectMessage = 7,
    DestroyGameObjectMessage = 8,
};
