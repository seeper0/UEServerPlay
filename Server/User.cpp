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

    Server->SendSpawnAllExceptSelf(Socket);

    std::cerr << "Connected @" << Socket << std::endl;
}

User::~User()
{
    Packet::NtLeave Packet;
    Packet.UserId = UserId;
    Server->NotiPacket(Socket, &Packet);

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
    Packet.ServerTime = ServerNetwork::GetMilliseconds();
    Server->SendPacket(Socket, &Packet);
}

void User::OnRqMove(const Packet::RqMove* InPacket)
{
    if (Server == nullptr)
        return;

    Location = InPacket->Location;
    Direction = InPacket->Direction;
    FaceDirection = InPacket->FaceDirection;
    MoveMode = InPacket->MoveMode;
    Velocity = InPacket->Velocity;
    Acceleration = InPacket->Acceleration;

    // Notify
    {
        Packet::NtMove Packet;
        Packet.UserId = UserId;
        Packet.Location = Location;
        Packet.Direction = Direction;
        Packet.FaceDirection = FaceDirection;
        Packet.MoveMode = MoveMode;
        Packet.Velocity = Velocity;
        Packet.Acceleration = Acceleration;
        Packet.Timestamp = InPacket->Timestamp;

        Server->NotiPacket(Socket, &Packet);
    }
}

