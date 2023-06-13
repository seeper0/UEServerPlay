#include "ServerNetwork.h"
#include <iostream>
#include <list>
#include <process.h>
#include "User.h"

using std::cout;
using std::cerr;
using std::endl;

ServerNetwork::ServerNetwork()
{
    Initialize();
}

ServerNetwork::~ServerNetwork()
{
}

unsigned __stdcall ServerNetwork::StartRecvThread(void* data)
{
    ServerNetwork* Server = reinterpret_cast<ServerNetwork*>(data);
    Server->RecvThread();
    return 0;
}

void ServerNetwork::RecvThread()
{
    while (true)
    {
        Sleep(1);
        ThreadLock.lock();
        {
            std::list<User*> DisconnectedList;
            for (auto& Item : UserList)
            {
                User* CurrentUser = Item.second;
                int32 Error = ReceivePacket(CurrentUser->GetSocket());
                if (Error < 0 || Error == WSAEDISCON)
                {
                    DisconnectedList.push_back(CurrentUser);
                }
            }

            for (auto& Item : DisconnectedList)
            {
                SOCKET Socket = Item->GetSocket();
                delete Item;
                UserList.erase(Socket);
            }
        }
        ThreadLock.unlock();
    }
}

void ServerNetwork::Run()
{
    unsigned threadID;
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ServerNetwork::StartRecvThread, (void*)this, 0, &threadID);

    while (true)
    {
        Sleep(0);
        Accept();
    }
}

void ServerNetwork::NotiPacket(const uint64 InSocket, Packet::Header* InPacket)
{
    for (auto& Item : UserList)
    {
        User* CurrentUser = Item.second;
        if (CurrentUser->GetSocket() != InSocket)
        {
            SendPacket(CurrentUser->GetSocket(), InPacket);
        }
    }
}

void ServerNetwork::OnConnected(const uint64 InSocket)
{
}

void ServerNetwork::OnDisconnected(const uint64 InSocket)
{
}

#define CALL_PACKET(IN_SOCKET, PACKET_NAME) auto it = UserList.find(IN_SOCKET); if (it != UserList.end()) { it->second->On##PACKET_NAME(InPacket); }

void ServerNetwork::OnRqLogin(const uint64 InSocket, const Packet::RqLogin* InPacket)
{
    CALL_PACKET(InSocket, RqLogin);
}

void ServerNetwork::OnRqHeartbeat(const uint64 InSocket, const Packet::RqHeartbeat* InPacket)
{
    CALL_PACKET(InSocket, RqHeartbeat);
}

void ServerNetwork::OnRqMove(const uint64 InSocket, const Packet::RqMove* InPacket)
{
    CALL_PACKET(InSocket, RqMove);
}
#undef CALL_PACKET

int32 ServerNetwork::Initialize()
{
    WSADATA wsaData;
    const int32 IniResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (IniResult != 0)
    {
        cerr << "Can't Initialize winsock! Quitiing" << endl;
        return -1;
    }

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 3번 째 인자 0도 OK
    if (ListenSocket == INVALID_SOCKET)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        WSACleanup();
        return -2;
    }

    sockaddr_in SockAddr{}; // 기본 초기화 권장
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    SockAddr.sin_port = htons(SERVER_PORT);

    int32 BindResult = bind(ListenSocket, reinterpret_cast<sockaddr*>(&SockAddr), sizeof(SockAddr));
    if (BindResult == SOCKET_ERROR)
    {
        cerr << "Can't bind a socket! Quitting" << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return -3;
    }

    int32 ListenResult = listen(ListenSocket, SOMAXCONN);
    if (ListenResult == SOCKET_ERROR)
    {
        cerr << "Can't listen a socket! Quitting" << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return -4;
    }

    return 1;
}

SOCKET ServerNetwork::Accept()
{
    sockaddr_in ClientSockInfo;
    int32 ClientSize = sizeof(ClientSockInfo);
    SOCKET Socket = accept(ListenSocket, reinterpret_cast<SOCKADDR*>(&ClientSockInfo), &ClientSize);
    if (Socket != INVALID_SOCKET)
    {
        // 접속 정보라.. 딱히 필요 없는 부분
        char Host[NI_MAXHOST];
        char Service[NI_MAXHOST];
        ZeroMemory(Host, NI_MAXHOST);
        ZeroMemory(Service, NI_MAXHOST);

        if (getnameinfo((sockaddr*)&ClientSockInfo, sizeof(ClientSockInfo), Host, NI_MAXHOST, Service, NI_MAXSERV, 0) == 0)
        {
            cout << Host << " connected on port $" << Service << endl;
        }
        else
        {
            inet_ntop(AF_INET, &ClientSockInfo.sin_addr, Host, NI_MAXHOST);
            cout << Host << " connected on port %" << ntohs(ClientSockInfo.sin_port) << endl;
        }

        ThreadLock.lock();
        {
            User* NewUser = new User(this, Socket);
            UserList.insert({ Socket, NewUser });
        }
        ThreadLock.unlock();
    }
    return Socket;
}


void ServerNetwork::Cleanup()
{
    for (auto& Item : UserList)
    {
        User* CurrentUser = Item.second;
        delete CurrentUser;
    }
    UserList.clear();

    closesocket(ListenSocket);
    ListenSocket = INVALID_SOCKET;
    WSACleanup();
}
