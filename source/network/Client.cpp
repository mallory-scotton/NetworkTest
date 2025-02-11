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
#include "network/Client.hpp"
#include <sys/socket.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
Client::Client(void)
    : m_connected(false)
{}

///////////////////////////////////////////////////////////////////////////////
Client::Client(const std::string& address, Uint32 port)
    : m_connected(false)
{
    this->connect(address, port);
}

///////////////////////////////////////////////////////////////////////////////
Client::~Client()
{
    this->disconnect();
}

///////////////////////////////////////////////////////////////////////////////
bool Client::connect(const std::string& address, Uint32 port)
{
    if (m_connected)
        disconnect();

    std::cout << "Connecting to " << address << ':' << port << std::endl;

    m_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (m_socket == INVALID_SOCKET_VALUE) {
        std::cout << "Failed: Cannot create the socket" << std::endl;
        return (false);
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &addr.sin_addr);

    if (::connect(m_socket, (struct sockaddr*)&addr, sizeof(addr)) ==
        SOCKET_ERROR_VALUE) {
        std::cout << "Failed: Cannot connect to the server" << std::endl;
        closesocket(m_socket);
        return (false);
    }

#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(m_socket, FIONBIO, &mode);
#else
    fcntl(m_socket, F_SETFL, O_NONBLOCK);
#endif

    std::cout << "Connected!" << std::endl;

    m_connected = true;
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
void Client::disconnect(void)
{
    if (m_connected) {
        m_connected = false;
        closesocket(m_socket);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Client::isConnected(void) const
{
    return (m_connected);
}

///////////////////////////////////////////////////////////////////////////////
void Client::sendPacket(Packet& packet)
{
    if (!m_connected)
        return;
    send(m_socket, packet.data(), packet.size(), 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Client::receivePacket(Packet& packet)
{
    if (!m_connected)
        return (false);
    packet.clear();
    return (recv(m_socket, packet.data(), packet.MAX_SIZE, 0) > 0);
}

} // namespace tkd
