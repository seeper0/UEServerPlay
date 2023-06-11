#include "User.h"
#include "ServerNetwork.h"

User::User(class ServerNetwork* InServer, const SOCKET InSocket)
    : Server(InServer)
    , Socket(InSocket)
{
    RPC_STATUS Status = UuidCreate(&Id);
    if (Status != RPC_S_OK)
    {
        // 에러처리...
    }
}

void User::RqLogin(const Packet::RqLogin* Pkt)
{
}

void User::RqHeartbeat(const Packet::RqHeartbeat* Pkt)
{
}

void User::RqMove(const Packet::RqMove* Pkt)
{
}

