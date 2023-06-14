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
    uint64 GetUserId() const { return UserId; };
    const FVector& GetLocation() const { return Location; }
    const FVector& GetDirection() const { return Direction; }
    const FVector& GetFaceDirection() const { return FaceDirection; }

    void OnRqLogin(const Packet::RqLogin* InPacket);
    void OnRqHeartbeat(const Packet::RqHeartbeat* InPacket);
    void OnRqMove(const Packet::RqMove* InPacket);

private:
    ServerNetwork* Server = nullptr;
    SOCKET      Socket = INVALID_SOCKET;
    uint64      UserId = 0;
    FVector		Location;
    FVector		Direction;
    FVector		FaceDirection;
    uint32      MoveMode = 0;
    FVector		Velocity;
    FVector		Acceleration;
};

