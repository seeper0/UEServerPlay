#pragma once
#include "../Client/Source/Client/Network/Common/Packet.h"
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
    User(class ServerNetwork* InServer, const SOCKET InSocket, const WSAEVENT InRecvHandel);
    ~User();

    SOCKET GetSocket() const { return Socket; };
    WSAEVENT GetRecvHandel() const { return RecvHandel; };

    void OnRqLogin(const Packet::RqLogin* Pkt);
    void OnRqHeartbeat(const Packet::RqHeartbeat* Pkt);
    void OnRqMove(const Packet::RqMove* Pkt);

private:
    ServerNetwork* Server = nullptr;
    SOCKET      Socket = INVALID_SOCKET;
    WSAEVENT    RecvHandel = nullptr;
    uint64      UserId;
    FVector		Location;
    FVector		Direction;
};

