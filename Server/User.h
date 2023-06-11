#pragma once
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
    SOCKET GetSocket() const { return Socket; };

    void RqLogin(const Packet::RqLogin* Pkt);
    void RqHeartbeat(const Packet::RqHeartbeat* Pkt);
    void RqMove(const Packet::RqMove* Pkt);

private:
    ServerNetwork* Server = nullptr;
    UUID Id;
    SOCKET Socket = INVALID_SOCKET;
};

