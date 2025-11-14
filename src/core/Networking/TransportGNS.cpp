///
/// Created by thijs on 12-11-2025.
///

#include "core/Networking/TransportGNS.h"
#include <iostream>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <mutex>
#include <string>


TransportGNS *TransportGNS::pCallbackInstance = nullptr;

TransportGNS::TransportGNS()
    : hListenSock(k_HSteamListenSocket_Invalid)
      , hPollGroup(k_HSteamNetPollGroup_Invalid)
      , hConnection(k_HSteamNetConnection_Invalid)
      , pInterface(nullptr)
      , bIsServer(false)
      , nextConnectionId(1) {
    // Initialize GameNetworkingSockets
    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
        std::cerr << "GameNetworkingSockets_Init failed: " << errMsg << std::endl;
        return;
    }

    pInterface = SteamNetworkingSockets();

    // Set debug output
    SteamNetworkingUtils()->SetDebugOutputFunction(
        k_ESteamNetworkingSocketsDebugOutputType_Msg,
        [](ESteamNetworkingSocketsDebugOutputType eType, const char *pszMsg) {
            std::cout << pszMsg << std::endl;
        }
    );
}

TransportGNS::~TransportGNS() {
    shutdown();
    GameNetworkingSockets_Kill();
}

TransportResult TransportGNS::startServer(uint16_t port) {
    bIsServer = true;
    TransportResult result;

    SteamNetworkingIPAddr serverAddr;
    serverAddr.Clear();
    serverAddr.m_port = port;

    // Register connection status callback
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               (void *) steamNetConnectionStatusChangedCallback);

    // Create a listening socket
    hListenSock = pInterface->CreateListenSocketIP(serverAddr, 1, &opt);
    if (hListenSock == k_HSteamListenSocket_Invalid) {
        std::cerr << "Failed to listen on port " << port << std::endl;
        result.success = false;
        return result;
    }

    // Create a poll group for managing multiple connections
    hPollGroup = pInterface->CreatePollGroup();
    if (hPollGroup == k_HSteamNetPollGroup_Invalid) {
        std::cerr << "Failed to create poll group" << std::endl;
        result.success = false;
        return result;
    }

    std::cout << "Server listening on port " << port << std::endl;
    result.success = true;
    return result;
}


TransportResult TransportGNS::startClient(const char *serverAddress, uint16_t port) {
    bIsServer = false;
    TransportResult result;

    SteamNetworkingIPAddr serverAddr;
    if (!serverAddr.ParseString(serverAddress)) {
        std::cerr << "Invalid server address: " << serverAddress << std::endl;
        result.success = false;
        return result;
    }
    serverAddr.m_port = port;

    // Print resolved address
    char szAddr[SteamNetworkingIPAddr::k_cchMaxString];
    serverAddr.ToString(szAddr, sizeof(szAddr), true);
    std::cout << "Connecting to " << szAddr << std::endl;

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               (void *) steamNetConnectionStatusChangedCallback);

    // Attempt connection
    hConnection = pInterface->ConnectByIPAddress(serverAddr, 1, &opt);
    if (hConnection == k_HSteamNetConnection_Invalid) {
        std::cerr << "Failed to create connection" << std::endl;
        result.success = false;
        return result;
    }

    result.success = true;
    return result;
}


TransportResult TransportGNS::send(const RawMessage& msg) {
    EResult result;
    TransportResult transport_result;

    // Choose message reliability/ordering mode
    int sendFlags = 0;
    switch (msg.mode) {
        case SendMode::ReliableOrdered:
            sendFlags = k_nSteamNetworkingSend_Reliable;
            break;

        case SendMode::ReliableUnordered:
            // Slightly less delayed reliable send; still ordered per connection
            sendFlags = k_nSteamNetworkingSend_Reliable | k_nSteamNetworkingSend_UnreliableNoDelay;
            break;

        case SendMode::Unreliable:
            sendFlags = k_nSteamNetworkingSend_Unreliable;
            break;
    }

    if (bIsServer) {
        // Resolve connection handle
        HSteamNetConnection hConn = getSteamConnection(msg.connectionId);
        if (hConn == k_HSteamNetConnection_Invalid) {
            transport_result.success = false;
            return transport_result;
        }

        result = pInterface->SendMessageToConnection(
            hConn,
            msg.payload.data(),
            static_cast<uint32>(msg.payload.size()),
            sendFlags,
            nullptr
        );
    } else {
        result = pInterface->SendMessageToConnection(
            hConnection,
            msg.payload.data(),
            static_cast<uint32>(msg.payload.size()),
            sendFlags,
            nullptr
        );
    }

    transport_result.success = (result == k_EResultOK);
    return transport_result;
}


TransportResult TransportGNS::sendToAll(const RawMessage& msg) {
    TransportResult transport_result;

    auto clients = getActiveConnectionIds();
    for (int id: clients) {
        RawMessage rm = msg;
        rm.connectionId = id;
        TransportResult single_result = send(rm);
        if (!single_result.success) {
            transport_result.success = false;
            transport_result.errorCode = single_result.errorCode;
        }
    }

    return transport_result;
}


bool TransportGNS::disconnect(int connectionId) {
    // Close the given connection (server) or client link
    if (bIsServer) {
        HSteamNetConnection hConn = getSteamConnection(connectionId);
        if (hConn == k_HSteamNetConnection_Invalid) {
            return false;
        }

        pInterface->CloseConnection(hConn, 0, "Disconnected", true);
        mapConnections.erase(hConn);
    } else {
        if (hConnection != k_HSteamNetConnection_Invalid) {
            pInterface->CloseConnection(hConnection, 0, "Disconnected", true);
            hConnection = k_HSteamNetConnection_Invalid;
        }
    }

    return true;
}

void TransportGNS::poll() {
    // Handle both connection state changes and queued messages
    pollConnectionStateChanges();
    pollIncomingMessages();
}

void TransportGNS::shutdown() {
    // Gracefully close all active sockets and connections
    if (bIsServer) {
        // Close all connections
        for (auto &pair: mapConnections) {
            pInterface->CloseConnection(pair.first, 0, "Server Shutdown", true);
        }
        mapConnections.clear();

        if (hListenSock != k_HSteamListenSocket_Invalid) {
            pInterface->CloseListenSocket(hListenSock);
            hListenSock = k_HSteamListenSocket_Invalid;
        }

        if (hPollGroup != k_HSteamNetPollGroup_Invalid) {
            pInterface->DestroyPollGroup(hPollGroup);
            hPollGroup = k_HSteamNetPollGroup_Invalid;
        }
    } else {
        if (hConnection != k_HSteamNetConnection_Invalid) {
            pInterface->CloseConnection(hConnection, 0, "Client Shutdown", true);
            hConnection = k_HSteamNetConnection_Invalid;
        }
    }
}

void TransportGNS::pollIncomingMessages() {
    if (bIsServer) {
        while (true) {
            ISteamNetworkingMessage *pMsg = nullptr;
            int numMsgs = pInterface->ReceiveMessagesOnPollGroup(hPollGroup, &pMsg, 1);
            if (numMsgs == 0) break;

            int connId;
            {
                std::lock_guard<std::mutex> lock(mapMutex);
                connId = getConnectionId(pMsg->m_conn);
            }

            if (connId != -1 && onMessageReceived) {
                RawMessage rm(connId,
                              reinterpret_cast<const std::byte*>(pMsg->m_pData),
                              pMsg->m_cbSize);
                onMessageReceived(rm);
            }

            pMsg->Release();
        }
    } else {
        while (true) {
            ISteamNetworkingMessage *pMsg = nullptr;
            int numMsgs = pInterface->ReceiveMessagesOnConnection(hConnection, &pMsg, 1);
            if (numMsgs == 0) break;
            if (numMsgs < 0) {
                std::cerr << "Error receiving messages" << std::endl;
                break;
            }

            if (onMessageReceived) {
                RawMessage rm(0,
                              reinterpret_cast<const std::byte*>(pMsg->m_pData),
                              static_cast<size_t>(pMsg->m_cbSize));
                onMessageReceived(rm);
            }

            pMsg->Release();
        }
    }
}


void TransportGNS::pollConnectionStateChanges() {
    pCallbackInstance = this;
    pInterface->RunCallbacks();
}

void TransportGNS::steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo) {
    pCallbackInstance->onSteamNetConnectionStatusChanged(pInfo);
}

void TransportGNS::onSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo) {
    switch (pInfo->m_info.m_eState) {
        case k_ESteamNetworkingConnectionState_Connecting:
            if (bIsServer) {
                std::cout << "[Server] Connection request from " << pInfo->m_info.m_szConnectionDescription << std::endl;

                if (pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK) {
                    pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                    std::cerr << "[Server] Failed to accept connection" << std::endl;
                    break;
                }

                if (!pInterface->SetConnectionPollGroup(pInfo->m_hConn, hPollGroup)) {
                    pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                    std::cerr << "[Server] Failed to set poll group" << std::endl;
                    break;
                }

                int connId;
                {
                    std::lock_guard<std::mutex> lock(mapMutex);
                    connId = nextConnectionId++;
                    mapConnections[pInfo->m_hConn] = connId;
                }

                // signal connection event 
                if (onConnectionChanged) {
                    onConnectionChanged(connId, true);
                }

                std::cout << "[Server] Client assigned ID " << connId << std::endl;
            }
            break;

        case k_ESteamNetworkingConnectionState_Connected:
            if (!bIsServer) {
                std::cout << "Connected to server" << std::endl;
                if (onConnectionChanged) {
                    onConnectionChanged(0, true);
                }
            }
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
            // Handle disconnection
            if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
                std::cout << "Connection closed: " << pInfo->m_info.m_szEndDebug << std::endl;

                if (bIsServer) {
                    int connId = getConnectionId(pInfo->m_hConn);
                    if (connId != -1) {
                        mapConnections.erase(pInfo->m_hConn);
                        if (onConnectionChanged) {
                            onConnectionChanged(connId, false);
                        }
                    }
                } else {
                    hConnection = k_HSteamNetConnection_Invalid;
                    if (onConnectionChanged) {
                        onConnectionChanged(0, false);
                    }
                }
            }

            pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
            break;

        default:
            break;
    }
}

int TransportGNS::getConnectionId(HSteamNetConnection hConn) {
    auto it = mapConnections.find(hConn);
    return (it != mapConnections.end()) ? it->second : -1;
}

HSteamNetConnection TransportGNS::getSteamConnection(int connectionId) {
    for (const auto &pair: mapConnections) {
        if (pair.second == connectionId) {
            return pair.first;
        }
    }
    return k_HSteamNetConnection_Invalid;
}


std::vector<int> TransportGNS::getActiveConnectionIds() {
    std::lock_guard<std::mutex> lock(mapMutex);
    std::vector<int> ids;
    for (auto &pair: mapConnections) ids.push_back(pair.second);
    return ids;
}
