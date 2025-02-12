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
#include "PlayState.hpp"
#include "utils/Macros.hpp"
#include "states/MenuState.hpp"
#include "imgui/imgui.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::States
///////////////////////////////////////////////////////////////////////////////
namespace tkd::States
{

///////////////////////////////////////////////////////////////////////////////
void Play::init(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Play::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Left)  m_left = true;
        if (event.key.code == sf::Keyboard::Right) m_right = true;
        if (event.key.code == sf::Keyboard::Up)    m_up = true;
        if (event.key.code == sf::Keyboard::Down)  m_down = true;
    }
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Left)  m_left = false;
        if (event.key.code == sf::Keyboard::Right) m_right = false;
        if (event.key.code == sf::Keyboard::Up)    m_up = false;
        if (event.key.code == sf::Keyboard::Down)  m_down = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Play::handlePacket(Packet packet)
{
    Packet::Type type;

    packet >> type;

    switch (type) {
        case Packet::Type::PlayerList:
        {
            size_t size;
            packet >> size;
            for (size_t i = 0; i < size; i++) {
                int id = -1; Vec2f pos;
                packet >> id >> pos;
                m_enemies[id] = std::make_unique<Object>(sf::Color::Red);
                m_enemies[id]->setPosition(pos);
            }
            break;
        }
        case Packet::Type::PlayerJoined:
        {
            int id = -1; Vec2f pos;
            packet >> id >> pos;
            m_enemies[id] = std::make_unique<Object>(sf::Color::Red);
            m_enemies[id]->setPosition(pos);
            break;
        }
        case Packet::Type::PlayerLeft:
        {
            int id = -1;
            packet >> id;
            m_enemies.erase(id);
            break;
        }
        case Packet::Type::PlayerMove:
        {
            int id = -1; Vec2f pos;
            packet >> id >> pos;
            if (m_enemies.count(id))
                m_enemies[id]->setPosition(pos);
            break;
        }
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Play::update(float deltaT)
{
    if (*m_debug) {
        ImGui::Begin("Parameters");
        ImGui::SliderFloat("Speed", &m_speed, 0.f, 10000.f);
        ImGui::End();
    }

    if (!m_client->isConnected()) {
        m_manager->change(std::make_unique<States::Menu>());
        return;
    }

    if (m_left || m_right || m_up || m_down) {
        Vec2f movement;

        if (m_left)  movement.x -= 1.f;
        if (m_right) movement.x += 1.f;
        if (m_up)    movement.y -= 1.f;
        if (m_down)  movement.y += 1.f;

        movement *= deltaT;
        movement *= m_speed;

        m_player.move(movement);

        tkd::Packet packet(
            tkd::Packet::Type::PlayerMove,
            m_player.getPosition()
        );
        m_client->sendPacket(packet);
    }

    m_player.update();
    for (const auto& [id, enemy] : m_enemies)
        enemy->update();
}

///////////////////////////////////////////////////////////////////////////////
void Play::render(void)
{
    m_player.render(*m_window);
    for (const auto& [id, enemy] : m_enemies)
        enemy->render(*m_window);
}

} // namespace tkd::States
