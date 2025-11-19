//
// Created by Lieven Schokker on 07/11/2025.
//

#include <iostream>

#include "Networking/Client.h"
#include "Networking/Server/Server.h"
#include "Networking/Server/ServerInformation.h"

int main()
{
    ServerConnectionInformation serverInformation{8080, "127.0.0.1"};
    Server server{serverInformation};
    Client client{};

    server.start();


    std::atomic<bool> running{true};

    std::thread serverBackgroundThread([&server, &running]() {
        while (running) {
            server.update();
        }
    });

    client.connectToServer(8080, "127.0.0.1");

    std::cout << "Hello, World!" << std::endl;
    std::cout << "Server running. Press Enter to stop..." << std::endl;
    std::cin.get();

    running = false;
    serverBackgroundThread.join();
    return 0;
}
