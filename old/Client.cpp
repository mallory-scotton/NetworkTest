#include <SFML/Graphics.hpp>
#include "PacketTypes.hpp"
#include <iostream>

class GameClient {
public:
    GameClient() : window(sf::VideoMode(800, 600), "SFML Network Game"), playerId(-1) {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET_VALUE) {
            throw std::runtime_error("Failed to create client socket");
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(55001);
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR_VALUE) {
            closesocket(clientSocket);
            throw std::runtime_error("Failed to connect to server");
        }

        // Set non-blocking
        #ifdef _WIN32
            u_long mode = 1;
            ioctlsocket(clientSocket, FIONBIO, &mode);
        #else
            fcntl(clientSocket, F_SETFL, O_NONBLOCK);
        #endif

        // Initialize player shape
        playerShape.setRadius(20.f);
        playerShape.setFillColor(sf::Color::Green);
        playerShape.setPosition(100, 100);
    }

    ~GameClient() {
        closesocket(clientSocket);
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            handleNetworkMessages();
            render();
        }
    }

private:
    sf::RenderWindow window;
    SocketType clientSocket;
    sf::CircleShape playerShape;
    std::map<int, sf::CircleShape> otherPlayers;
    int playerId;
    NetworkInitializer netInit;

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2f movement(0, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  movement.x -= 5;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) movement.x += 5;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    movement.y -= 5;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  movement.y += 5;

        if (movement.x != 0 || movement.y != 0) {
            playerShape.move(movement);
            
            GamePacket packet(PacketType::PlayerMove);
            packet.position = playerShape.getPosition();
            sendPacket(packet);
        }
    }

    void handleNetworkMessages() {
        Packet packet;
        int result = recv(clientSocket, packet.data.data(), packet.MAX_SIZE, 0);
        
        if (result > 0) {
            GamePacket gamePacket;
            gamePacket.deserialize(packet);

            switch (gamePacket.type) {
                case PacketType::PlayerList: {
                    // Handle player list...
                    break;
                }
                case PacketType::PlayerJoined:
                    addPlayer(gamePacket.playerId, gamePacket.position);
                    break;
                case PacketType::PlayerLeft:
                    otherPlayers.erase(gamePacket.playerId);
                    break;
                case PacketType::PlayerMove:
                    if (otherPlayers.count(gamePacket.playerId)) {
                        otherPlayers[gamePacket.playerId].setPosition(gamePacket.position);
                    }
                    break;
            }
        }
    }

    void addPlayer(int pid, const sf::Vector2f& position) {
        sf::CircleShape shape;
        shape.setRadius(20.f);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(position);
        otherPlayers[pid] = shape;
    }

    void sendPacket(const GamePacket& gamePacket) {
        Packet packet;
        gamePacket.serialize(packet);
        send(clientSocket, packet.data.data(), packet.readPos, 0);
    }

    void render() {
        window.clear(sf::Color::Black);
        window.draw(playerShape);
        
        for (const auto& player : otherPlayers) {
            window.draw(player.second);
        }
        
        window.display();
    }
};

int main() {
    try {
        GameClient client;
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
