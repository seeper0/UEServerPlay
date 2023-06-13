#pragma once
#include "../Client/Source/Client/Network/Packet.h"
#include <WS2tcpip.h>

namespace Packet
{
    struct RqLogin;
    struct RqHeartbeat;
    struct RqMove;
}

class User
{
public:
    User(class ServerNetwork* InServer, const SOCKET InSocket);
    ~User();

    SOCKET GetSocket() const { return Socket; };

    void OnRqLogin(const Packet::RqLogin* InPacket);
    void OnRqHeartbeat(const Packet::RqHeartbeat* InPacket);
    void OnRqMove(const Packet::RqMove* InPacket);

private:
    ServerNetwork* Server = nullptr;
    SOCKET      Socket = INVALID_SOCKET;
    uint64      UserId;
    FVector		Location;
    FVector		Direction;
    FVector		FaceDirection;
};

