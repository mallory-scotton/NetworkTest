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
#include "game/Player.hpp"
#include "utils/Macros.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
Player::Player(void)
    : Character(Vec2f(400.f, 50.f), 100.f)
{
    Vec2f dimension(10.f, 20.f);

    m_shape.setPosition(m_position);
    m_shape.setSize(dimension);
    m_shape.setFillColor(sf::Color::Red);

    m_collider = std::make_unique<Collider>(m_position, dimension);
}

///////////////////////////////////////////////////////////////////////////////
void Player::update(float deltaT)
{
    IGNORE(deltaT);
    m_shape.setPosition(m_position);
}

///////////////////////////////////////////////////////////////////////////////
void Player::handleCollision(Entity* other)
{
    IGNORE(other);
}

///////////////////////////////////////////////////////////////////////////////
void Player::render(sf::RenderWindow& window)
{
    window.draw(m_shape);
}

} // namespace tkd
