// main.cpp (FIXED)
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <set>
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"
#include "Networking/Server/Server.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/Messages/ConnectionMessage.h"

std::atomic<bool> clientConnected{false};
std::atomic<int> firstClientId{-1};

void runServer()
{
    ServerConnectionInformation serverInformation{8080, "127.0.0.1"};
    Server server{serverInformation};

    server.start();

    std::atomic<bool> running{true};
    bool hasKickedClient = false;

    std::cout << " Waiting for clients to connect" << std::endl;

    auto kickTimer = std::chrono::steady_clock::now();
    bool kickTimerStarted = false;

    while (running)
    {
        server.update();

        if (!kickTimerStarted && clientConnected.load())
        {
            kickTimerStarted = true;
            kickTimer = std::chrono::steady_clock::now();
            std::cout << "Client connected!" << std::endl;
        }

        if (kickTimerStarted && !hasKickedClient)
        {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - kickTimer).count();

            if (elapsed > 2000)
            {
                int clientId = firstClientId.load();
                if (clientId != -1)
                {
                    std::cout << "Auto-kicking client " << clientId << std::endl;
                    server.kickClient(clientId);
                    hasKickedClient = true;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void runClient()
{

    Client client{};

    std::cout << "Connecting to server" << std::endl;

    if (!client.connectToServer(8080, "127.0.0.1"))
    {
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (client.isConnected())
    {
        std::cout << "Successfully connected!" << std::endl;

        ConnectionMessage message;
        message.setStatus(ConnectionStatus::Connected);
        client.sendMessage(message);

        std::cout << "Sent connection message" << std::endl;
    }


    for (int i = 0; i < 100; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (!client.isConnected())
        {
            std::cout << "Disconnected by server" << std::endl;
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    // Check command line arguments
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <mode>\n";
        std::cout << "  mode: 'server' or 'client'\n";
        std::cout << "\nExamples:\n";
        std::cout << "  " << argv[0] << " server\n";
        std::cout << "  " << argv[0] << " client\n";
        return 1;
    }

    std::string mode = argv[1];

    // Convert to lowercase for easier comparison
    for (auto& c : mode)
        c = std::tolower(c);

    if (mode == "server")
    {
        runServer();
    }
    else if (mode == "client")
    {
        runClient();
    }
    else
    {
        std::cerr << "Error: Unknown mode '" << argv[1] << "'\n";
        std::cerr << "Please use 'server' or 'client'\n";
        return 1;
    }

    return 0;
}