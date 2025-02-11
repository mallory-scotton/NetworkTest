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

#ifdef NEON_SERVER

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "network/Server.hpp"
#include <iostream>
#include <thread>
#include <signal.h>
#include <atomic>

///////////////////////////////////////////////////////////////////////////////
std::atomic<bool> running(true);

///////////////////////////////////////////////////////////////////////////////
void signalHandler(int signum)
{
    std::cout << "\nShutdown signal received (" << signum << ")." << std::endl;
    running = false;
}

///////////////////////////////////////////////////////////////////////////////
int main(void)
{
    std::cout << "Starting game server..." << std::endl;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        tkd::Server server(55001);

        std::thread sthread([&server](void){
            try {
                while (running) {
                    server.update();
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
            } catch (const std::exception& e) {
                std::cerr << "Server thread error: " << e.what() << std::endl;
                running = false;
            }
        });

        while (running)
            server.run();
        std::cout << "Shutting down server..." << std::endl;
        if (sthread.joinable())
            sthread.join();
        std::cout << "Server shutdown complete." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}

#else

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "utils/Types.hpp"
#include "network/Client.hpp"

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

    void move(const tkd::Vec2f& direction)
    {
        m_position += direction * 2.f;
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
int main(void)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Network", sf::Style::Close);
    sf::Event event;
    tkd::Client client("127.0.0.1", 55001);
    Object player;
    std::map<int, std::unique_ptr<Object>> enemies;

    bool UP = false, DOWN = false, LEFT = false, RIGHT = false;

    window.setFramerateLimit(60);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left)  LEFT = true;
                if (event.key.code == sf::Keyboard::Right) RIGHT = true;
                if (event.key.code == sf::Keyboard::Up)    UP = true;
                if (event.key.code == sf::Keyboard::Down)  DOWN = true;
            }
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Left)  LEFT = false;
                if (event.key.code == sf::Keyboard::Right) RIGHT = false;
                if (event.key.code == sf::Keyboard::Up)    UP = false;
                if (event.key.code == sf::Keyboard::Down)  DOWN = false;
            }
            if (event.type == sf::Event::MouseMoved) {
                tkd::Vec2f pos(event.mouseMove.x, event.mouseMove.y);

                player.setPosition(pos);
                tkd::Packet packet(tkd::Packet::Type::PlayerMove, pos);
                client.sendPacket(packet);
            }
        }

        if (UP || DOWN || LEFT || RIGHT) {
            tkd::Vec2f movement;

            if (UP)     movement.y -= 1.f;
            if (DOWN)   movement.y += 1.f;
            if (LEFT)   movement.x -= 1.f;
            if (RIGHT)  movement.x += 1.f;

            player.move(movement);

            tkd::Packet packet(tkd::Packet::Type::PlayerMove, player.getPosition());
            client.sendPacket(packet);
        }

        {
            tkd::Packet packet;

            while (client.receivePacket(packet)) {

                tkd::Packet::Type type;
                packet >> type;

                switch (type) {
                    case tkd::Packet::Type::PlayerList:
                    {
                        size_t size;
                        packet >> size;
                        for (size_t i = 0; i < size; i++) {
                            int id = -1; tkd::Vec2f pos;
                            packet >> id >> pos;
                            enemies[id] = std::make_unique<Object>(sf::Color::Red);
                            enemies[id]->setPosition(pos);
                        }
                        break;
                    }
                    case tkd::Packet::Type::PlayerJoined:
                    {
                        int id = -1; tkd::Vec2f pos;
                        packet >> id >> pos;
                        enemies[id] = std::make_unique<Object>(sf::Color::Red);
                        enemies[id]->setPosition(pos);
                        break;
                    }
                    case tkd::Packet::Type::PlayerLeft:
                    {
                        int id = -1;
                        packet >> id;
                        enemies.erase(id);
                        break;
                    }
                    case tkd::Packet::Type::PlayerMove:
                    {
                        int id = -1; tkd::Vec2f pos;
                        packet >> id >> pos;
                        if (enemies.count(id))
                            enemies[id]->setPosition(pos);
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        player.update();
        for (const auto& [id, enemy] : enemies)
            enemy->update();

        window.clear();

        player.render(window);
        for (const auto& [id, enemy] : enemies)
            enemy->render(window);

        window.display();
    }
}

#endif
