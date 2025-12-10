#include "Core/GameWorld.h"
#include "Networking/Client.h"
#include "Networking/Server/Server.h"
#include "Networking/Messages/IMessage.h"

<<<<<<< HEAD
bool GameWorld::sendToServer(const IMessage& message)
=======
bool GameWorld::sendToServer(const IMessage& message) const
>>>>>>> origin/development
{
    if (client)
    {
        return client->sendMessage(message);
    }
    return false;
}

<<<<<<< HEAD
bool GameWorld::broadcastToClients(const IMessage& message)
=======
bool GameWorld::broadcastToClients(const IMessage& message) const
>>>>>>> origin/development
{
    if (server)
    {
        return server->broadcastMessage(message);
    }
    return false;
}

<<<<<<< HEAD
bool GameWorld::sendToClient(int clientId, const IMessage& message)
=======
bool GameWorld::sendToClient(int clientId, const IMessage& message) const
>>>>>>> origin/development
{
    if (server)
    {
        return server->sendMessage(clientId, message);
    }
    return false;
<<<<<<< HEAD
}
=======
}
>>>>>>> origin/development
