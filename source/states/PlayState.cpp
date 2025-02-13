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
        if (event.key.code == sf::Keyboard::Space) m_up = true;
    }
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Left)  m_left = false;
        if (event.key.code == sf::Keyboard::Right) m_right = false;
        if (event.key.code == sf::Keyboard::Space) m_up = false;
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
                m_enemies[id] = std::make_unique<Player>();
                m_enemies[id]->setPosition(pos);
            }
            break;
        }
        case Packet::Type::PlayerJoined:
        {
            int id = -1; Vec2f pos;
            packet >> id >> pos;
            m_enemies[id] = std::make_unique<Player>();
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
        case Packet::Type::Disconnect:
        {
            m_manager->change(std::make_unique<States::Menu>());
            break;
        }
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Play::update(float deltaT)
{
    if (!m_client->isConnected()) {
        m_manager->change(std::make_unique<States::Menu>());
        return;
    }

    static const float MOVE_FORCE = 50000.f;
    static const float JUMP_FORCE = 6000.f;

    if (m_right)
        m_player.applyForce(Vec2f(MOVE_FORCE, 0.f), deltaT);
    if (m_left)
        m_player.applyForce(Vec2f(-MOVE_FORCE, 0.f), deltaT);
    if (m_up && m_player.onGround())
        m_player.setVelocity(Vec2f(m_player.getVelocity().x, -JUMP_FORCE));

    if (!m_player.getVelocity().equals(0.f, 0.001f)) {
        tkd::Packet packet(
            tkd::Packet::Type::PlayerMove,
            m_player.getPosition()
        );
        m_client->sendPacket(packet);
    }

    m_player.update(deltaT);
    m_player.updatePhysics(deltaT, m_room);
    for (const auto& [id, enemy] : m_enemies)
        enemy->update(deltaT);
}

///////////////////////////////////////////////////////////////////////////////
void Play::render(void)
{
    m_room.render(*m_window);
    m_player.render(*m_window);
    for (const auto& [id, enemy] : m_enemies)
        enemy->render(*m_window);
}

} // namespace tkd::States
