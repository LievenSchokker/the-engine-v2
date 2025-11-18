//
// Created by thijs on 18-11-2025.
//


#include "Client.h"
#include <iostream>

// Thread-safe console input
std::string GetConsoleInput()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}


Client::Client()
{
    const uint16_t DEFAULT_PORT = 27020;
    const char* DEFAULT_SERVER_IP = "127.0.0.1";

    // --- 1. Message callback ---
    setDefaultOnMessageReceived();

    // --- 2. Connection state callback ---
    SetDefaultOnConnectionChanged();

    // --- 3. Initiate connection ---
    if (connectToServer(DEFAULT_PORT, DEFAULT_SERVER_IP)) return;

    // --- 4. Input thread ---
    std::thread inputThread;
    createInputThread(inputThread);

    // --- 5. Main poll loop ---
    std::thread listenThread;
    createListenThread(listenThread);

    // --- 6. Cleanup ---
    transport->closeOpenSocket();

    if (inputThread.joinable())
        inputThread.join();

    std::cout << "[Client] Shutdown complete.\n";
}


void Client::SetOnConnectionChanged(const OnConnectionChangedCallback& newCallBack) const
{
    if (newCallBack)
    {
        transport->setOnConnectionChanged(newCallBack);
        return;
    }
}


void Client::SetDefaultOnConnectionChanged()
{
    transport->setOnConnectionChanged([&](int assignedId, bool isConnected)
    {
        if (isConnected)
        {
            connected = true;
            clientConnectionId = assignedId;

            std::cout << "[Client] Connected. Assigned ID: "
                << clientConnectionId << "\n";
            std::cout << "Type messages. /quit to disconnect.\n";
        }
        else
        {
            connected = false;
            running = false;
            std::cout << "[Client] Disconnected.\n";
        }
    });
}


void Client::setDefaultOnMessageReceived() const
{
    transport->setOnMessageReceived([&](const RawMessage& msg)
    {
        std::cout << "[Server]: " << msg.toString() << std::endl;
    });
}


void Client::setDefaultOnMessageReceived(const OnMessageReceivedCallback& newCallback) const
{
    transport->setOnMessageReceived(newCallback);
}


bool Client::connectToServer(const uint16_t port, const char* serverIP)
{
    std::cout << "[Client] Connecting to " << serverIP
        << ":" << port << "...\n";

    TransportResult tr = transport->connectByIPAdress(serverIP, port);
    if (!tr.success)
    {
        std::cerr << "[Client] Failed to initiate connection.\n";
        return true;
    }
    running = true;
    return false;
}


void Client::createInputThread(std::thread& inputThread)
{
    inputThread = std::thread([&]()
    {
        while (running)
        {
            std::string input = GetConsoleInput();

            if (input == "/quit")
            {
                running = false;
                transport->disconnectFromSocket(1);
                break;
            }

            if (connected)
            {
                RawMessage msg(1, input);
                transport->send(msg);
            }
        }
    });
}


void Client::createListenThread(std::thread& ListenThread) const
{
    ListenThread = std::thread([&]()
    {
        while (running)
        {
            transport->poll();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}
