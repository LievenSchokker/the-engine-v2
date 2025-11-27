#include "Core/GameWorld.h"
#include "Networking/Client.h"
#include "Networking/Server/Server.h"
#include "Networking/Messages/IMessage.h"

bool GameWorld::sendToServer(const IMessage& message)
{
    if (client)
    {
        return client->sendMessage(message);
    }
    return false;
}

bool GameWorld::broadcastToClients(const IMessage& message)
{
    if (server)
    {
        return server->broadcastMessage(message);
    }
    return false;
}

bool GameWorld::sendToClient(int clientId, const IMessage& message)
{
    if (server)
    {
        return server->sendMessage(clientId, message);
    }
    return false;
}