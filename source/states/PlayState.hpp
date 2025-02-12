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
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "GameState.hpp"
#include <map>
#include <memory>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::States
///////////////////////////////////////////////////////////////////////////////
namespace tkd::States
{

class Object
{
private:
    sf::Color m_color;
    tkd::Vec2f m_position;
    sf::CircleShape m_shape;

public:
    Object(sf::Color color = sf::Color::Green)
        : m_color(color)
        , m_position(0.f)
    {
        m_shape.setRadius(20.f);
        m_shape.setOrigin(Vec2f(10.f));
        m_shape.setFillColor(color);
    }

public:
    void update(void)
    {
        m_shape.setPosition(m_position);
    }

    void render(sf::RenderWindow& window)
    {
        window.draw(m_shape);
    }

    void move(const tkd::Vec2f& velocity)
    {
        m_position += velocity;

        if (m_position.x < 10) m_position.x = 10;
        if (m_position.x > 790) m_position.x = 790;
        if (m_position.y < 10) m_position.y = 10;
        if (m_position.y > 590) m_position.y = 590;
    }

    tkd::Vec2f getPosition(void) const
    {
        return (m_position);
    }

    void setPosition(const tkd::Vec2f& position)
    {
        m_position = position;
    }
};

///////////////////////////////////////////////////////////////////////////////
/// \brief play state class
///
///////////////////////////////////////////////////////////////////////////////
class Play : public GameState
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Private properties
    ///////////////////////////////////////////////////////////////////////////
    bool m_left = false;
    bool m_right = false;
    bool m_up = false;
    bool m_down = false;
    Object m_player;
    std::map<int, std::unique_ptr<Object>> m_enemies;
    float m_speed = 150.f;

public:
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    void init(void) override;

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    void handleEvent(sf::Event event) override;

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    void handlePacket(Packet packet) override;

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    void update(float deltaT) override;

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    void render(void) override;
};

} // namespace tkd::States
