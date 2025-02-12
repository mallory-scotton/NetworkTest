///////////////////////////////////////////////////////////////////////////////
///
/// MIT License
///
/// Copyright(c) 2025 TekyoDrift
///
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following coditions:
///
/// The above copyright notice and this permission notice shall be included
/// in all copies or substantial portions of the Software?
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "ServerDiscovery.hpp"
#include <iostream>
#include <cstring>
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define closesocket close
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
ServerDiscovery::ServerDiscovery(Uint16 gamePort)
    : m_running(false)
    , m_gamePort(gamePort)
    , m_socket(INVALID_SOCKET_VALUE)
{}

///////////////////////////////////////////////////////////////////////////////
ServerDiscovery::~ServerDiscovery()
{
    this->stop();
}

///////////////////////////////////////////////////////////////////////////////
void ServerDiscovery::startBroadcasting(void)
{
    if (m_running)
        return;
    m_running = true;

    m_thread = std::thread([this](void)
    {
        m_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_socket < 0) {
            std::cerr << "Failed to create discovery socket" << std::endl;
            return;
        }

        int broadcast = 1;
        setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, 
                   (char*)&broadcast, sizeof(broadcast));

        sockaddr_in broadcastAddr;
        broadcastAddr.sin_family = AF_INET;
        broadcastAddr.sin_port = htons(DISCOVERY_PORT);
        broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

        char message[256];
        snprintf(message, sizeof(message), "%s:%d", 
                DISCOVERY_MESSAGE, m_gamePort);

        while (m_running) {
            sendto(m_socket, message, strlen(message), 0,
                   (struct sockaddr*)&broadcastAddr, 
                   sizeof(broadcastAddr));

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        closesocket(m_socket);
        m_socket = INVALID_SOCKET_VALUE;
    });
}

///////////////////////////////////////////////////////////////////////////////
void ServerDiscovery::startListening(ServerFoundCallback callback)
{
    if (m_running)
        return;
    m_running = true;
    m_callback = callback;

    m_thread = std::thread([this](void)
    {
        m_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_socket < 0) {
            std::cerr << "Failed to create discovery socket" << std::endl;
            return;
        }

    #ifdef _WIN32
        unsigned long mode = 1;
        ioctlsocket(m_socket, FIONBIO, &mode);
    #else
        int flags = fcntl(m_socket, F_GETFL, 0);
        fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
    #endif

        sockaddr_in listenAddr;
        listenAddr.sin_family = AF_INET;
        listenAddr.sin_port = htons(DISCOVERY_PORT);
        listenAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(m_socket, (struct sockaddr*)&listenAddr, 
                sizeof(listenAddr)) < 0) {
            std::cerr << "Failed to bind discovery socket" << std::endl;
            closesocket(m_socket);
            m_socket = INVALID_SOCKET_VALUE;
            return;
        }

        while (m_running) {
            char buffer[256];
            sockaddr_in senderAddr;
            socklen_t senderLen = sizeof(senderAddr);

            int received = recvfrom(m_socket, buffer, sizeof(buffer), 0,
                                  (struct sockaddr*)&senderAddr, &senderLen);

            if (received > 0) {
                buffer[received] = '\0';

                char expectedMsg[256];
                int port;
                if (sscanf(buffer, "%[^:]:%d", expectedMsg, &port) == 2 &&
                    strcmp(expectedMsg, DISCOVERY_MESSAGE) == 0) {
                    char ipStr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(senderAddr.sin_addr), 
                             ipStr, INET_ADDRSTRLEN);

                    if (m_callback)
                        m_callback(ipStr, port);
                }
            }
        }

        closesocket(m_socket);
        m_socket = INVALID_SOCKET_VALUE;
    });
}

///////////////////////////////////////////////////////////////////////////////
void ServerDiscovery::stop(void)
{
    if (!m_running)
        return;
    m_running = false;
    if (m_socket != INVALID_SOCKET_VALUE) {
    #ifdef _WIN32
        shutdown(m_socket, SD_BOTH);
    #else
        shutdown(m_socket, SHUT_RDWR);
    #endif
    }
    if (m_thread.joinable())
        m_thread.join();
}

///////////////////////////////////////////////////////////////////////////////
const char* ServerDiscovery::DISCOVERY_MESSAGE = "TEKYODRIFT_GAME_SERVER";

} // namespace tkd
