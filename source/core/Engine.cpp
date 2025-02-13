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
#include "core/Engine.hpp"
#include "states/MenuState.hpp"
#include "states/TestState.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
Engine::Engine(bool debug)
    : m_window(sf::VideoMode(800, 600), "MyNeonAbyss", sf::Style::Close)
    , m_debug(debug)
    , m_manager(m_window, &m_client, &m_debug)
{
    m_manager.push(std::make_unique<States::Test>());
}

///////////////////////////////////////////////////////////////////////////////
void Engine::renderFpsDisplay(void) const
{
    static const ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav;
    bool opened = true;

    if (m_debug) {
        auto& io = ImGui::GetIO();

        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGui::Begin("FPS", &opened, window_flags);
        ImGui::Text(
            "FPS: %.2f (%.2gms)",
            io.Framerate,
            io.Framerate ? 1000.0f / io.Framerate : 0.0f
        );
        ImGui::End();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Engine::start(void)
{
    sf::Event event;
    tkd::Packet packet;
    sf::Clock clock;
    sf::Time restart;
    float deltaT;

    if (!ImGui::SFML::Init(m_window))
        exit(84);
    if (!m_debug)
        ImGui::SFML::Update(m_window, sf::Time());

    while (m_window.isOpen()) {
        restart = clock.restart();

        deltaT = restart.asSeconds();

        if (m_debug)
            ImGui::SFML::Update(m_window, restart);

        while (m_window.pollEvent(event)) {
            if (m_debug)
                ImGui::SFML::ProcessEvent(m_window, event);
            if (event.type == sf::Event::Closed)
                m_window.close();
            if (
                event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Num0
            ) m_debug = !m_debug;
            m_manager.handleEvent(event);
        }

        while (m_client.receivePacket(packet))
            m_manager.handlePacket(packet);

        m_manager.update(deltaT);

        renderFpsDisplay();

        m_window.clear();
        m_manager.render();
        if (m_debug)
            ImGui::SFML::Render(m_window);
        m_window.display();

        if (m_manager.empty())
            m_window.close();
    }

    while (!m_manager.empty())
        m_manager.pop();

    ImGui::SFML::Shutdown(m_window);
}

} // namespace tkd
