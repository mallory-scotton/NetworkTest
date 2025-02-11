#pragma once
#include "NetworkUtils.hpp"

enum class PacketType : int {
    Connect,
    Disconnect,
    PlayerMove,
    PlayerList,
    PlayerJoined,
    PlayerLeft
};

class GamePacket {
public:
    GamePacket() {}
    GamePacket(PacketType type) : type(type) {}

    PacketType type;
    sf::Vector2f position;
    int playerId;

    void serialize(Packet& packet) const {
        packet << static_cast<int>(type) << playerId << position;
    }

    void deserialize(Packet& packet) {
        int typeInt;
        packet >> typeInt >> playerId >> position;
        type = static_cast<PacketType>(typeInt);
    }
};
