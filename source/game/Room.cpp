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
#include "game/Room.hpp"
#include "utils/Constants.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// TEMPORARY
Room::Room(void)
{
    {
        Vec2f position(0.f, 500.f);
        Vec2f dimension(800.f, 20.f);

        m_colliders.push_back(Collider(position, dimension));
        m_tiles.push_back(sf::RectangleShape());

        m_tiles[0].setFillColor(sf::Color::Green);
        m_tiles[0].setPosition(position);
        m_tiles[0].setSize(dimension);
    }

    {
        Vec2f position(100.f, 0.f);
        Vec2f dimension(20.f, 400.f);

        m_colliders.push_back(Collider(position, dimension));
        m_tiles.push_back(sf::RectangleShape());

        m_tiles[1].setFillColor(sf::Color::Blue);
        m_tiles[1].setPosition(position);
        m_tiles[1].setSize(dimension);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Room::checkCollision(const Collider& collider) const
{
    for (const auto& bound : m_colliders) {
        if (bound | collider)
            return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
Vec2f Room::resolveCollision(const Collider& collider) const
{
    Vec2f resolution(0.f);

    for (const auto& bound : m_colliders) {
        if (!(bound | collider))
            continue;
        Vec2f bpos = bound.getPosition();
        Vec2f bdim = bound.getDimension();
        Vec2f cpos = collider.getPosition();
        Vec2f cdim = collider.getDimension();

        Vec2f bmid = bpos + (bdim * .5f);
        Vec2f cmid = cpos + (cdim * .5f);

        Vec2f overlap(
            (bdim.x + cdim.x) * .5f - std::abs(bmid.x - cmid.x),
            (bdim.y + cdim.y) * .5f - std::abs(bmid.y - cmid.y)
        );

        static const float EPSILON = 0.0001f;

        if (overlap.x <= EPSILON && overlap.y <= EPSILON)
            continue;

        Vec2f direction(
            (cmid.x < bmid.x) ? -1.f : 1.f,
            (cmid.y < bmid.y) ? -1.f : 1.f
        );

        if (overlap.x < overlap.y)
            resolution.x += overlap.x * direction.x;
        else
            resolution.y += overlap.y * direction.y;
    }

    return (resolution);
}

///////////////////////////////////////////////////////////////////////////////
void Room::render(sf::RenderWindow& window)
{
    for (const auto& tile : m_tiles)
        window.draw(tile);
}

} // namespace tkd
