#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <map>
#include <vector>
#include <mutex>
#include "../Client/Source/Client/Network/Common/Network.h"

class ServerNetwork final : public Network
{
public:
    ServerNetwork();
    virtual ~ServerNetwork();

public:
    void Run();
    void NotiPacket(const uint64 InSocket, Packet::Header* InPacket);

protected:
    virtual void OnConnected(const uint64 InSocket) override;
    virtual void OnDisconnected(const uint64 InSocket) override;
    virtual void OnRqLogin(const uint64 InSocket, const Packet::RqLogin* InPacket) override;
    virtual void OnRqHeartbeat(const uint64 InSocket, const Packet::RqHeartbeat* InPacket) override;
    virtual void OnRqMove(const uint64 InSocket, const Packet::RqMove* InPacket) override;

private:
    static unsigned __stdcall StartRecvThread(void* data);
    void RecvThread();

private:
    int32 Initialize();
    SOCKET Accept();
    void Cleanup();

    SOCKET ListenSocket = INVALID_SOCKET;
    std::map<SOCKET, class User*> UserList;
    std::mutex ThreadLock;
};

