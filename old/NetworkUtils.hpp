#pragma once
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketType;
    #define SOCKET_ERROR_VALUE SOCKET_ERROR
    #define INVALID_SOCKET_VALUE INVALID_SOCKET
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    typedef int SocketType;
    #define SOCKET_ERROR_VALUE (-1)
    #define INVALID_SOCKET_VALUE (-1)
    #define closesocket close
#endif

#include <SFML/System/Vector2.hpp>
#include <vector>
#include <cstring>

class NetworkInitializer {
public:
    NetworkInitializer() {
        #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
        #endif
    }
    
    ~NetworkInitializer() {
        #ifdef _WIN32
        WSACleanup();
        #endif
    }
};

// Custom packet serialization
struct Packet {
    static const size_t MAX_SIZE = 1024;
    std::vector<char> data;
    size_t readPos = 0;

    Packet() : data(MAX_SIZE) {}

    void clear() {
        data.clear();
        data.resize(MAX_SIZE);
        readPos = 0;
    }

    template<typename T>
    Packet& operator<<(const T& value) {
        size_t size = sizeof(T);
        if (data.size() < size) {
            data.resize(size);
        }
        std::memcpy(data.data() + readPos, &value, size);
        readPos += size;
        return *this;
    }

    template<typename T>
    Packet& operator>>(T& value) {
        size_t size = sizeof(T);
        if (readPos + size <= data.size()) {
            std::memcpy(&value, data.data() + readPos, size);
            readPos += size;
        }
        return *this;
    }

    // Special handling for sf::Vector2f
    Packet& operator<<(const sf::Vector2f& vec) {
        *this << vec.x << vec.y;
        return *this;
    }

    Packet& operator>>(sf::Vector2f& vec) {
        *this >> vec.x >> vec.y;
        return *this;
    }
};
