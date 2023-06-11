#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <rpc.h>
#include <map>
#include "../Client/Source/Client/Network/Common/Network.h"

class ServerNetwork final : public Network
{
public:
    ServerNetwork();
    virtual ~ServerNetwork();

public:
    void Run();

protected:
    virtual void RqLogin(const uint64 InSocket, const Packet::RqLogin* Pkt) override;
    virtual void RqHeartbeat(const uint64 InSocket, const Packet::RqHeartbeat* Pkt) override;
    virtual void RqMove(const uint64 InSocket, const Packet::RqMove* Pkt) override;

private:
    int32 Initialize();
    SOCKET Accept();
    void AddNewUser(const SOCKET NewSocket);
    void Cleanup();

    SOCKET ServerSocket = INVALID_SOCKET;
    sockaddr_in ClientSockInfo;
    std::map<SOCKET, class User*> UserList;
};

