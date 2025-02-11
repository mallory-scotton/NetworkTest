#include <iostream>
#include <map>
#include <set>
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <thread>
#include <atomic>
#include "PacketTypes.hpp"

class GameServer {
public:
    GameServer(unsigned short port) : nextPlayerId(1) {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET_VALUE) {
            throw std::runtime_error("Failed to create server socket");
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR_VALUE) {
            closesocket(serverSocket);
            throw std::runtime_error("Failed to bind server socket");
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR_VALUE) {
            closesocket(serverSocket);
            throw std::runtime_error("Failed to listen on server socket");
        }

        // Set non-blocking
        #ifdef _WIN32
            u_long mode = 1;
            ioctlsocket(serverSocket, FIONBIO, &mode);
        #else
            fcntl(serverSocket, F_SETFL, O_NONBLOCK);
        #endif

        std::cout << "Server started on port " << port << std::endl;
    }

    ~GameServer() {
        for (const auto& client : clients) {
            closesocket(client.second);
        }
        closesocket(serverSocket);
    }

    void run() {
        while (true) {
            handleNewConnections();
            handleClientMessages();
        }
    }

    void update(void) {}

private:
    SocketType serverSocket;
    std::map<int, SocketType> clients;
    std::map<int, sf::Vector2f> playerPositions;
    int nextPlayerId;
    NetworkInitializer netInit;

    void handleNewConnections() {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        
        SocketType clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket != INVALID_SOCKET_VALUE) {
            int playerId = nextPlayerId++;
            
            // Set non-blocking
            #ifdef _WIN32
                u_long mode = 1;
                ioctlsocket(clientSocket, FIONBIO, &mode);
            #else
                fcntl(clientSocket, F_SETFL, O_NONBLOCK);
            #endif

            // Send current player list
            Packet packet;
            GamePacket playerListPacket(PacketType::PlayerList);
            playerListPacket.serialize(packet);
            send(clientSocket, packet.data.data(), packet.readPos, 0);

            // Add new client
            clients[playerId] = clientSocket;
            playerPositions[playerId] = sf::Vector2f(100, 100);
            
            // Notify other clients
            GamePacket joinPacket(PacketType::PlayerJoined);
            joinPacket.playerId = playerId;
            joinPacket.position = sf::Vector2f(100, 100);
            broadcastPacket(joinPacket);

            std::cout << "Client " << playerId << " connected" << std::endl;
        }
    }

    void handleClientMessages() {
        for (auto it = clients.begin(); it != clients.end();) {
            Packet packet;
            int result = recv(it->second, packet.data.data(), packet.MAX_SIZE, 0);
            
            if (result > 0) {
                GamePacket gamePacket;
                gamePacket.deserialize(packet);
                
                switch (gamePacket.type) {
                    case PacketType::PlayerMove:
                        playerPositions[it->first] = gamePacket.position;
                        gamePacket.playerId = it->first;
                        broadcastPacket(gamePacket);
                        break;
                    default:
                        break;
                }
                ++it;
            }
            else if (result == 0 || (result < 0 && 
                #ifdef _WIN32
                    WSAGetLastError() != WSAEWOULDBLOCK
                #else
                    errno != EWOULDBLOCK && errno != EAGAIN
                #endif
            )) {
                // Client disconnected
                handleClientDisconnection(it++);
            }
            else {
                ++it;
            }
        }
    }

    void handleClientDisconnection(std::map<int, SocketType>::iterator it) {
        int playerId = it->first;
        closesocket(it->second);
        clients.erase(it);
        playerPositions.erase(playerId);

        GamePacket packet(PacketType::PlayerLeft);
        packet.playerId = playerId;
        broadcastPacket(packet);

        std::cout << "Client " << playerId << " disconnected" << std::endl;
    }

    void broadcastPacket(const GamePacket& gamePacket) {
        Packet packet;
        gamePacket.serialize(packet);
        
        for (const auto& client : clients) {
            send(client.second, packet.data.data(), packet.readPos, 0);
        }
    }
};

std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "\nShutdown signal received (" << signum << ")." << std::endl;
    running = false;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [port]" << std::endl;
    std::cout << "Default port: 55001" << std::endl;
}

int main(int argc, char* argv[]) {
    unsigned short port = 55001; // Default port

    // Parse command line arguments
    if (argc > 1) {
        if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        
        try {
            port = static_cast<unsigned short>(std::stoi(argv[1]));
        } catch (const std::exception& e) {
            std::cerr << "Invalid port number: " << argv[1] << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Set up signal handling
    signal(SIGINT, signalHandler);  // Handle Ctrl+C
    signal(SIGTERM, signalHandler); // Handle termination request

    try {
        std::cout << "Starting game server..." << std::endl;
        GameServer server(port);

        // Create a separate thread for server operations
        std::thread serverThread([&server]() {
            try {
                while (running) {
                    server.update(); // Assuming we've added an update method that processes one frame
                    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 updates per second
                }
            } catch (const std::exception& e) {
                std::cerr << "Server thread error: " << e.what() << std::endl;
                running = false;
            }
        });

        // Main thread can handle console commands or monitoring
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // Could add console commands here if desired
        }

        // Clean shutdown
        std::cout << "Shutting down server..." << std::endl;
        serverThread.join();
        std::cout << "Server shutdown complete." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
