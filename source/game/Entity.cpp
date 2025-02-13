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
#include "game/Entity.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
Entity::Entity(const Vec2f& position)
    : m_position(position)
    , m_onAir(false)
{}

///////////////////////////////////////////////////////////////////////////////
void Entity::updatePhysics(float deltaT, const Room& room)
{
    static const float GRAVITY = 981.f;
    static const float FRICTION = .001f;

    applyForce(Vec2f(0.f, GRAVITY), deltaT);

    float frictionFactor = std::pow(FRICTION, deltaT);
    m_velocity.x *= frictionFactor;

    m_position += m_velocity * deltaT;

    if (m_collider)
        m_collider->setPosition(m_position);
    handleCollision(room);
}

///////////////////////////////////////////////////////////////////////////////
void Entity::handleCollision(const Room& room)
{
    m_onAir = true;
    if (!m_collider || !room.checkCollision(*m_collider))
        return;

    Vec2f resolution = room.resolveCollision(*m_collider);

    m_position += resolution;
    m_collider->setPosition(m_position);

    if (resolution.x != 0.f)
        m_velocity.x = 0.f;
    if (resolution.y != 0.f) {
        m_velocity.y = 0.f;
        if (resolution.y < 0.f)
            m_onAir = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Entity::applyForce(const Vec2f& force, float deltaT)
{
    static const float MAX_VELOCITY = 500.f;

    m_velocity += force * deltaT;

    m_velocity.x = std::clamp(m_velocity.x, -MAX_VELOCITY, MAX_VELOCITY);
    m_velocity.y = std::clamp(m_velocity.y, -MAX_VELOCITY, MAX_VELOCITY);
}

///////////////////////////////////////////////////////////////////////////////
bool Entity::onGround(void) const
{
    return (!m_onAir);
}

///////////////////////////////////////////////////////////////////////////////
void Entity::setPosition(const Vec2f& position)
{
    m_position = position;
}

///////////////////////////////////////////////////////////////////////////////
Vec2f Entity::getPosition(void) const
{
    return (m_position);
}

///////////////////////////////////////////////////////////////////////////////
Vec2f Entity::getVelocity(void) const
{
    return (m_velocity);
}

///////////////////////////////////////////////////////////////////////////////
void Entity::setVelocity(const Vec2f& velocity)
{
    m_velocity = velocity;
}

} // namespace tkd
