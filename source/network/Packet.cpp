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
#include "network/Packet.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
Packet::Packet(void)
{
    m_data.resize(MAX_SIZE);
    memset(m_data.data(), 0, MAX_SIZE);
}

///////////////////////////////////////////////////////////////////////////////
Packet::Packet(Type type)
{
    m_data.resize(MAX_SIZE);
    memset(m_data.data(), 0, MAX_SIZE);
    *this << type;
}

///////////////////////////////////////////////////////////////////////////////
Packet::Packet(const Packet& other)
    : m_rpos(other.m_rpos)
    , m_wpos(other.m_wpos)
{
    m_data = other.m_data;
}

///////////////////////////////////////////////////////////////////////////////
const Byte* Packet::data(void) const
{
    return (m_data.data());
}

///////////////////////////////////////////////////////////////////////////////
Byte* Packet::data(void)
{
    return (m_data.data());
}

///////////////////////////////////////////////////////////////////////////////
size_t Packet::size(void)
{
    return (m_wpos);
}

///////////////////////////////////////////////////////////////////////////////
void Packet::clear(void)
{
    m_data.clear();
    m_data.resize(MAX_SIZE);
    m_rpos = 0;
    m_wpos = 0;
}

} // namespace tkd
