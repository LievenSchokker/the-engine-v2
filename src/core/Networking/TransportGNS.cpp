///
/// Created by thijs on 12-11-2025.
///

#include "../../../include/core/Networking/TransportGNS.h"
#include <iostream>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

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

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               (void *) steamNetConnectionStatusChangedCallback);

    hListenSock = pInterface->CreateListenSocketIP(serverAddr, 1, &opt);
    if (hListenSock == k_HSteamListenSocket_Invalid) {
        std::cerr << "Failed to listen on port " << port << std::endl;
        result.success = false;
        return result;
    }

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

    char szAddr[SteamNetworkingIPAddr::k_cchMaxString];
    serverAddr.ToString(szAddr, sizeof(szAddr), true);
    std::cout << "Connecting to " << szAddr << std::endl;

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               (void *) steamNetConnectionStatusChangedCallback);

    hConnection = pInterface->ConnectByIPAddress(serverAddr, 1, &opt);
    if (hConnection == k_HSteamNetConnection_Invalid) {
        std::cerr << "Failed to create connection" << std::endl;
        result.success = false;
        return result;
    }

    result.success = true;
    return result;
}

TransportResult TransportGNS::send(int connectionId, const std::byte data[], size_t length, bool reliable) {
    EResult result;
    TransportResult transport_result;

    if (bIsServer) {
        HSteamNetConnection hConn = getSteamConnection(connectionId);
        if (hConn == k_HSteamNetConnection_Invalid) {
            transport_result.success = false;
            return transport_result;
        }

        result = pInterface->SendMessageToConnection(
            hConn,
            data,
            (uint32) length,
            reliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable,
            nullptr
        );
    } else {
        result = pInterface->SendMessageToConnection(
            hConnection,
            data,
            (uint32) length,
            reliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable,
            nullptr
        );
    }

    if (result == k_EResultOK) {
        transport_result.success = true;
        return transport_result;
    }
    else {
        transport_result.success = false;
        return transport_result;
    }
}

bool TransportGNS::disconnect(int connectionId) {
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
    pollConnectionStateChanges();
    pollIncomingMessages();
}

void TransportGNS::shutdown() {
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
            if (numMsgs < 0) {
                std::cerr << "Error receiving messages" << std::endl;
                break;
            }

            int connId = getConnectionId(pMsg->m_conn);
            if (connId != -1 && onMessageReceived) {
                onMessageReceived(connId, (const std::byte *) pMsg->m_pData, pMsg->m_cbSize);
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
                onMessageReceived(0, (const std::byte *) pMsg->m_pData, pMsg->m_cbSize);
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
                std::cout << "Connection request from " << pInfo->m_info.m_szConnectionDescription << std::endl;

                if (pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK) {
                    pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                    std::cerr << "Failed to accept connection" << std::endl;
                    break;
                }

                if (!pInterface->SetConnectionPollGroup(pInfo->m_hConn, hPollGroup)) {
                    pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                    std::cerr << "Failed to set poll group" << std::endl;
                    break;
                }

                int connId = nextConnectionId++;
                mapConnections[pInfo->m_hConn] = connId;

                if (onConnectionChanged) {
                    onConnectionChanged(connId, true);
                }
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
