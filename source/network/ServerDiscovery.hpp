#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <functional>

namespace tkd {

class ServerDiscovery {
public:
    static const uint16_t DISCOVERY_PORT = 55000;  // Port for discovery messages
    static const char* DISCOVERY_MESSAGE;          // Magic message to identify our game

    // Callback type for when a server is discovered
    using ServerFoundCallback = std::function<void(const std::string& address, uint16_t port)>;

private:
    std::atomic<bool> m_running;
    std::thread m_thread;
    ServerFoundCallback m_callback;
    uint16_t m_gamePort;  // The actual game server port

public:
    ServerDiscovery(uint16_t gamePort) : m_running(false), m_gamePort(gamePort) {}
    ~ServerDiscovery() { stop(); }

    // Start broadcasting server presence
    void startBroadcasting();
    
    // Start listening for servers
    void startListening(ServerFoundCallback callback);
    
    // Stop broadcasting/listening
    void stop();
};

} // namespace tkd
