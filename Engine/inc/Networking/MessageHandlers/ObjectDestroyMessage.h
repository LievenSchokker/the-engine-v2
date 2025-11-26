#pragma once


class NetworkSpawnManager;

class ObjectDestroyMessageHandler : public IMessageHandler
{
public:
    explicit ObjectDestroyMessageHandler(NetworkSpawnManager& spawnManager);
    void handle(const IMessage& message) override;

private:
    NetworkSpawnManager& spawnManager;
};