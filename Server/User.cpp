#include "User.h"
#include "ServerNetwork.h"
#include <iostream>

User::User(class ServerNetwork* InServer, const SOCKET InSocket)
    : Server(InServer)
    , Socket(InSocket)
    , UserId(InSocket) // 대충 uuid 생성이라 치자
{
    u_long NonBlockingMode = 1;
    ioctlsocket(Socket, FIONBIO, &NonBlockingMode);

    std::cerr << "Connected @" << Socket << std::endl;
}

User::~User()
{
    std::cerr << "Disconnected User @" << Socket << std::endl;
    closesocket(Socket);
}

void User::OnRqLogin(const Packet::RqLogin* InPacket)
{
    if (Server == nullptr)
        return;

    Location = FVector(900, 1110, 93);
    Direction = FVector(0, 0, 0);

    // Response
    {
        Packet::RpLogin Packet;
        Packet.UserId = UserId;
        Packet.Location = Location;
        Packet.Direction = Direction;
        Server->SendPacket(Socket, &Packet);
    }

    // Notify
    {
        Packet::NtSpawn Packet;
        Packet.UserId = UserId;
        Packet.Location = Location;
        Packet.Direction = Direction;
        Server->NotiPacket(Socket, &Packet);
    }
}

void User::OnRqHeartbeat(const Packet::RqHeartbeat* InPacket)
{
    if (Server == nullptr)
        return;

    Packet::RpHeartbeat Packet;
    Packet.Timestamp = GetTickCount64();
    Server->SendPacket(Socket, &Packet);
}

void User::OnRqMove(const Packet::RqMove* InPacket)
{
    if (Server == nullptr)
        return;

    // Notify
    {
        Packet::NtMove Packet;
        Packet.UserId = UserId;
        Packet.Location = Location;
        Packet.Direction = Direction;
        Server->NotiPacket(Socket, &Packet);
    }
}

