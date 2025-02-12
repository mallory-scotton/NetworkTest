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
#include "DiscoveryState.hpp"
#include "utils/Macros.hpp"
#include "states/PlayState.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::States
///////////////////////////////////////////////////////////////////////////////
namespace tkd::States
{

///////////////////////////////////////////////////////////////////////////////
void Discovery::init(void)
{
    m_discovery.startListening(
        [this](const std::string& address, Uint16 port)
        {
            if (m_servers.count(address))
                return;
            m_servers[address] = port;
            std::cout << "Found: " << address << ':' << port << std::endl;
        }
    );

    m_shape.setSize({200, 50});
    m_shape.setOrigin({100, 25});
    m_shape.setFillColor(sf::Color::Green);
}

///////////////////////////////////////////////////////////////////////////////
void Discovery::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed) {
        Uint32 idx = 0;
        Vec2f pos(event.mouseButton.x, event.mouseButton.y);

        for (auto [address, port] : m_servers) {
            m_shape.setPosition({400, 50 + 75 * (float)idx});
            idx++;
            if (
                m_shape.getGlobalBounds().contains(pos) &&
                m_client->connect(address, port)
            ) {
                m_manager->change(std::make_unique<States::Play>());
                break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Discovery::handlePacket(Packet packet)
{
    IGNORE(packet);
}

///////////////////////////////////////////////////////////////////////////////
void Discovery::update(float deltaT)
{
    IGNORE(deltaT);
}

///////////////////////////////////////////////////////////////////////////////
void Discovery::render(void)
{
    Uint32 idx = 0;

    for (auto [address, port] : m_servers) {
        m_shape.setPosition({400, 50 + 75 * (float)idx});
        m_window->draw(m_shape);
        idx++;
    }
}

} // namespace tkd::States
