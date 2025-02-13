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
#include "utils/Vec2.hpp"
#include "physics/Collider.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Room
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Private room properties
    ///////////////////////////////////////////////////////////////////////////
    std::vector<sf::RectangleShape> m_tiles;    //<! The tiles set
    std::vector<Collider> m_colliders;          //<! The collision box

public:
    // TEMPORARY
    Room(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check the collision with another collider
    ///
    /// \param collider The collider to check collision with
    ///
    /// \return True if the collider collides with the room
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool checkCollision(const Collider& collider) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Resolve the collision with a collider
    ///
    /// \param collider The other collider
    ///
    /// \return The resolved collision
    ///
    ///////////////////////////////////////////////////////////////////////////
    Vec2f resolveCollision(const Collider& collider) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Render the room on the window
    ///
    /// \param window The window to display on
    ///
    ///////////////////////////////////////////////////////////////////////////
    void render(sf::RenderWindow& window);
};

} // namespace tkd
