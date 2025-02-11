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
#include "network/Server.hpp"
#include <iostream>
#include <set>
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <thread>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
Server::Server(Uint32 port)
    : m_nextPlayerId(0)
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (m_socket == INVALID_SOCKET_VALUE)
        throw std::runtime_error("Failed to create server socket");

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(m_socket,
            (struct sockaddr*)&serverAddr,
            sizeof(serverAddr)
        ) == SOCKET_ERROR_VALUE) {
        closesocket(m_socket);
        throw std::runtime_error("Failed to bind server socket");
    }

    if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR_VALUE) {
        closesocket(m_socket);
        throw std::runtime_error("Failed to listen on server socket");
    }

#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(serverSocket, FIONBIO, &mode);
#else
    fcntl(m_socket, F_SETFL, O_NONBLOCK);
#endif

    std::cout << "Server started on port " << port << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
Server::~Server()
{
    for (const auto& client : m_clients)
        closesocket(client.second->socket);
    closesocket(m_socket);
}

///////////////////////////////////////////////////////////////////////////////
void Server::run(void)
{
    handleNewConnections();
    handleClientMessages();
}

///////////////////////////////////////////////////////////////////////////////
void Server::update(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Server::handleNewConnections(void)
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    Socket socket = accept(m_socket, (struct sockaddr*)&addr, &len);

    if (socket != INVALID_SOCKET_VALUE) {
        int id = m_nextPlayerId++;

    #ifdef _WIN32
        u_long mode = 1;
        ioctlsocket(socket, FIONBIO, &mode);
    #else
        fcntl(socket, F_SETFL, O_NONBLOCK);
    #endif

        {
            Packet packet(Packet::Type::PlayerList);

            packet << m_clients.size();
            for (const auto& client : m_clients)
                packet << client.first << client.second->position;
            send(socket, packet.data(), packet.size(), 0);
        }

        m_clients[id] = std::make_unique<ClientInfo>(socket, Vec2f(0.f));

        {
            Packet packet(Packet::Type::PlayerJoined);

            packet << id << Vec2f(0.f);
            broadcastPacket(packet, socket);
        }

        std::cout << "Client " << id << " connected" << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Server::handleClientMessages(void)
{
    for (auto it = m_clients.begin(); it != m_clients.end();) {
        Packet packet;
        int res = recv(it->second->socket, packet.data(), packet.MAX_SIZE, 0);

        if (res > 0) {
            Packet::Type type;
            Packet result;

            packet >> type;
            result << type;

            switch (type) {
                case Packet::Type::PlayerMove:
                {
                    packet >> m_clients[it->first]->position;
                    result << it->first << m_clients[it->first]->position;
                    broadcastPacket(result, m_clients[it->first]->socket);
                    break;
                }
                default:
                {
                    break;
                }
            }

            ++it;
        } else if (res == 0 || (res < 0 &&
        #ifdef _WIN32
            WSAGetLastError() != WSAEWOULDBLOCK
        #else
            errno != EWOULDBLOCK && errno != EAGAIN
        #endif
        )) {
            handleDisconnections(it++);
        } else {
            ++it;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Server::handleDisconnections(std::map<int, std::unique_ptr<ClientInfo>>::iterator it)
{
    int id = it->first;
    Packet packet(Packet::Type::PlayerLeft, id);

    closesocket(it->second->socket);
    m_clients.erase(it);
    broadcastPacket(packet, it->second->socket);
    std::cout << "Client " << id << " disconnected" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void Server::broadcastPacket(Packet& packet, Socket socket)
{
    for (const auto& client : m_clients) {
        if (client.second->socket != socket)
            send(client.second->socket, packet.data(), packet.size(), 0);
    }
}

} // namespace tkd
