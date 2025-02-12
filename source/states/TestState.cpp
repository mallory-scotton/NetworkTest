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
#include "TestState.hpp"
#include "utils/Macros.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::States
///////////////////////////////////////////////////////////////////////////////
namespace tkd::States
{

///////////////////////////////////////////////////////////////////////////////
void Test::init(void)
{
    // Here you can init every shapes/objects you will use such as:
    // player|enemy|shapes|collider|levels...
}

///////////////////////////////////////////////////////////////////////////////
void Test::handleEvent(sf::Event event)
{
    IGNORE(event);
    // Here represent the loop after polling event from the window
    // You can process each event separatly and for a better coding style
    // You can create private function to handle some specific events
}

///////////////////////////////////////////////////////////////////////////////
void Test::handlePacket(Packet packet)
{
    IGNORE(packet);
    // Here is the handling of the packet, for now, you don't need to worry
    // about this part, only the great jesus lord know how this is working
}

///////////////////////////////////////////////////////////////////////////////
void Test::update(float deltaT)
{
    IGNORE(deltaT);
    // Updates ! Here all the movement, collision, calculation are done
    // NO DISPLAY ! Except for ImGui that need to be encapsulated within
    // the *m_debug property
}

///////////////////////////////////////////////////////////////////////////////
void Test::render(void)
{
    // Rendering, this is for rendering only, if someone see a calculation here
    // you can blame the author of this code !
    // Throw them rocks !
}

} // namespace tkd::States
