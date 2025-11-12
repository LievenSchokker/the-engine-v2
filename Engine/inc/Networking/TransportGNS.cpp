///
/// Created by thijs on 12-11-2025.
///

#include "TransportGNS.h"
#include <iostream>
#include <cstring>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>


TransportGNS* TransportGNS::s_pCallbackInstance = nullptr;

TransportGNS::TransportGNS()
    : m_hListenSock(k_HSteamListenSocket_Invalid)
    , m_hPollGroup(k_HSteamNetPollGroup_Invalid)
    , m_hConnection(k_HSteamNetConnection_Invalid)
    , m_pInterface(nullptr)
    , m_bIsServer(false)
    , m_nextConnectionId(1)
{
    // Initialize GameNetworkingSockets
    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
        std::cerr << "GameNetworkingSockets_Init failed: " << errMsg << std::endl;
        return;
    }

    m_pInterface = SteamNetworkingSockets();

    // Set debug output
    SteamNetworkingUtils()->SetDebugOutputFunction(
        k_ESteamNetworkingSocketsDebugOutputType_Msg,
        [](ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg) {
            std::cout << pszMsg << std::endl;
        }
    );
}

TransportGNS::~TransportGNS() {
    Shutdown();
    GameNetworkingSockets_Kill();
}

bool TransportGNS::StartServer(uint16_t port) {
    m_bIsServer = true;

    SteamNetworkingIPAddr serverAddr;
    serverAddr.Clear();
    serverAddr.m_port = port;

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               (void*)SteamNetConnectionStatusChangedCallback);

    m_hListenSock = m_pInterface->CreateListenSocketIP(serverAddr, 1, &opt);
    if (m_hListenSock == k_HSteamListenSocket_Invalid) {
        std::cerr << "Failed to listen on port " << port << std::endl;
        return false;
    }

    m_hPollGroup = m_pInterface->CreatePollGroup();
    if (m_hPollGroup == k_HSteamNetPollGroup_Invalid) {
        std::cerr << "Failed to create poll group" << std::endl;
        return false;
    }

    std::cout << "Server listening on port " << port << std::endl;
    return true;
}

bool TransportGNS::StartClient(const char* serverAddress, uint16_t port) {
    m_bIsServer = false;

    SteamNetworkingIPAddr serverAddr;
    if (!serverAddr.ParseString(serverAddress)) {
        std::cerr << "Invalid server address: " << serverAddress << std::endl;
        return false;
    }
    serverAddr.m_port = port;

    char szAddr[SteamNetworkingIPAddr::k_cchMaxString];
    serverAddr.ToString(szAddr, sizeof(szAddr), true);
    std::cout << "Connecting to " << szAddr << std::endl;

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               (void*)SteamNetConnectionStatusChangedCallback);

    m_hConnection = m_pInterface->ConnectByIPAddress(serverAddr, 1, &opt);
    if (m_hConnection == k_HSteamNetConnection_Invalid) {
        std::cerr << "Failed to create connection" << std::endl;
        return false;
    }

    return true;
}

bool TransportGNS::Send(int connectionId, const std::byte data[], size_t length, bool reliable) {
    EResult result;

    if (m_bIsServer) {
        HSteamNetConnection hConn = GetSteamConnection(connectionId);
        if (hConn == k_HSteamNetConnection_Invalid) {
            return false;
        }

        result = m_pInterface->SendMessageToConnection(
            hConn,
            data,
            (uint32)length,
            reliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable,
            nullptr
        );
    } else {
        result = m_pInterface->SendMessageToConnection(
            m_hConnection,
            data,
            (uint32)length,
            reliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable,
            nullptr
        );
    }

    return result == k_EResultOK;
}

bool TransportGNS::Disconnect(int connectionId) {
    if (m_bIsServer) {
        HSteamNetConnection hConn = GetSteamConnection(connectionId);
        if (hConn == k_HSteamNetConnection_Invalid) {
            return false;
        }

        m_pInterface->CloseConnection(hConn, 0, "Disconnected", true);
        m_mapConnections.erase(hConn);
    } else {
        if (m_hConnection != k_HSteamNetConnection_Invalid) {
            m_pInterface->CloseConnection(m_hConnection, 0, "Disconnected", true);
            m_hConnection = k_HSteamNetConnection_Invalid;
        }
    }

    return true;
}

void TransportGNS::Poll() {
    PollConnectionStateChanges();
    PollIncomingMessages();
}

void TransportGNS::Shutdown() {
    if (m_bIsServer) {
        // Close all connections
        for (auto& pair : m_mapConnections) {
            m_pInterface->CloseConnection(pair.first, 0, "Server Shutdown", true);
        }
        m_mapConnections.clear();

        if (m_hListenSock != k_HSteamListenSocket_Invalid) {
            m_pInterface->CloseListenSocket(m_hListenSock);
            m_hListenSock = k_HSteamListenSocket_Invalid;
        }

        if (m_hPollGroup != k_HSteamNetPollGroup_Invalid) {
            m_pInterface->DestroyPollGroup(m_hPollGroup);
            m_hPollGroup = k_HSteamNetPollGroup_Invalid;
        }
    } else {
        if (m_hConnection != k_HSteamNetConnection_Invalid) {
            m_pInterface->CloseConnection(m_hConnection, 0, "Client Shutdown", true);
            m_hConnection = k_HSteamNetConnection_Invalid;
        }
    }
}

void TransportGNS::PollIncomingMessages() {
    if (m_bIsServer) {
        while (true) {
            ISteamNetworkingMessage* pMsg = nullptr;
            int numMsgs = m_pInterface->ReceiveMessagesOnPollGroup(m_hPollGroup, &pMsg, 1);
            if (numMsgs == 0) break;
            if (numMsgs < 0) {
                std::cerr << "Error receiving messages" << std::endl;
                break;
            }

            int connId = GetConnectionId(pMsg->m_conn);
            if (connId != -1 && m_onMessageReceived) {
                m_onMessageReceived(connId, (const std::byte*)pMsg->m_pData, pMsg->m_cbSize);
            }

            pMsg->Release();
        }
    } else {
        while (true) {
            ISteamNetworkingMessage* pMsg = nullptr;
            int numMsgs = m_pInterface->ReceiveMessagesOnConnection(m_hConnection, &pMsg, 1);
            if (numMsgs == 0) break;
            if (numMsgs < 0) {
                std::cerr << "Error receiving messages" << std::endl;
                break;
            }

            if (m_onMessageReceived) {
                m_onMessageReceived(0, (const std::byte*)pMsg->m_pData, pMsg->m_cbSize);
            }

            pMsg->Release();
        }
    }
}

void TransportGNS::PollConnectionStateChanges() {
    s_pCallbackInstance = this;
    m_pInterface->RunCallbacks();
}

void TransportGNS::SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo) {
    s_pCallbackInstance->OnSteamNetConnectionStatusChanged(pInfo);
}

void TransportGNS::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) {
    switch (pInfo->m_info.m_eState) {
        case k_ESteamNetworkingConnectionState_Connecting:
            if (m_bIsServer) {
                std::cout << "Connection request from " << pInfo->m_info.m_szConnectionDescription << std::endl;

                if (m_pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK) {
                    m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                    std::cerr << "Failed to accept connection" << std::endl;
                    break;
                }

                if (!m_pInterface->SetConnectionPollGroup(pInfo->m_hConn, m_hPollGroup)) {
                    m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                    std::cerr << "Failed to set poll group" << std::endl;
                    break;
                }

                int connId = m_nextConnectionId++;
                m_mapConnections[pInfo->m_hConn] = connId;

                if (m_onConnectionChanged) {
                    m_onConnectionChanged(connId, true);
                }
            }
            break;

        case k_ESteamNetworkingConnectionState_Connected:
            if (!m_bIsServer) {
                std::cout << "Connected to server" << std::endl;
                if (m_onConnectionChanged) {
                    m_onConnectionChanged(0, true);
                }
            }
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
            if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
                std::cout << "Connection closed: " << pInfo->m_info.m_szEndDebug << std::endl;

                if (m_bIsServer) {
                    int connId = GetConnectionId(pInfo->m_hConn);
                    if (connId != -1) {
                        m_mapConnections.erase(pInfo->m_hConn);
                        if (m_onConnectionChanged) {
                            m_onConnectionChanged(connId, false);
                        }
                    }
                } else {
                    m_hConnection = k_HSteamNetConnection_Invalid;
                    if (m_onConnectionChanged) {
                        m_onConnectionChanged(0, false);
                    }
                }
            }

            m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
            break;

        default:
            break;
    }
}

int TransportGNS::GetConnectionId(HSteamNetConnection hConn) {
    auto it = m_mapConnections.find(hConn);
    return (it != m_mapConnections.end()) ? it->second : -1;
}

HSteamNetConnection TransportGNS::GetSteamConnection(int connectionId) {
    for (const auto& pair : m_mapConnections) {
        if (pair.second == connectionId) {
            return pair.first;
        }
    }
    return k_HSteamNetConnection_Invalid;
}