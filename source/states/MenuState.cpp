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
#include "MenuState.hpp"
#include "utils/Macros.hpp"
#include "states/DiscoveryState.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::States
///////////////////////////////////////////////////////////////////////////////
namespace tkd::States
{

///////////////////////////////////////////////////////////////////////////////
void Menu::init(void)
{
    m_button.setSize({200, 50});
    m_button.setOrigin({100, 25});
    m_button.setPosition({400, 300});
    m_button.setFillColor(sf::Color::Magenta);
}

///////////////////////////////////////////////////////////////////////////////
void Menu::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed) {
        Vec2f pos(event.mouseButton.x, event.mouseButton.y);

        if (m_button.getGlobalBounds().contains(pos))
            m_manager->change(std::make_unique<States::Discovery>());
    }

    if (event.type == sf::Event::MouseMoved) {
        Vec2f pos(event.mouseMove.x, event.mouseMove.y);

        if (m_button.getGlobalBounds().contains(pos))
            m_button.setScale(Vec2f(1.1f));
        else
            m_button.setScale(Vec2f(1.f));
    }
}

///////////////////////////////////////////////////////////////////////////////
void Menu::handlePacket(Packet packet)
{
    IGNORE(packet);
}

///////////////////////////////////////////////////////////////////////////////
void Menu::update(float deltaT)
{
    IGNORE(deltaT);
}

///////////////////////////////////////////////////////////////////////////////
void Menu::render(void)
{
    m_window->draw(m_button);
}

} // namespace tkd::States
