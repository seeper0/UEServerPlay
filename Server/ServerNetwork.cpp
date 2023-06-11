#include "ServerNetwork.h"
#include <iostream>
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

void ServerNetwork::Run()
{
    while (true)
    {
        Sleep(0);
        SOCKET NewSocket = Accept();
        if (NewSocket != INVALID_SOCKET)
        {
            AddNewUser(NewSocket);
        }
        else
        {
            for (auto& Item : UserList)
            {
                User* CurrentUser = Item.second;
                ReceivePacket(CurrentUser->GetSocket());
            }
        }
    }
}

#define CALL_PACKET(IN_SOCKET, PACKET_NAME) auto it = UserList.find(IN_SOCKET); if (it != UserList.end()) { it->second->PACKET_NAME(Pkt); }

void ServerNetwork::RqLogin(const uint64 InSocket, const Packet::RqLogin* Pkt)
{
    CALL_PACKET(InSocket, RqLogin);
}

void ServerNetwork::RqHeartbeat(const uint64 InSocket, const Packet::RqHeartbeat* Pkt)
{
    CALL_PACKET(InSocket, RqHeartbeat);
}

void ServerNetwork::RqMove(const uint64 InSocket, const Packet::RqMove* Pkt)
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

    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 3번 째 인자 0도 OK
    if (ServerSocket == INVALID_SOCKET)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        WSACleanup();
        return -2;
    }

    sockaddr_in SockAddr{}; // 기본 초기화 권장
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    SockAddr.sin_port = htons(SERVER_PORT);

    int32 BindResult = bind(ServerSocket, reinterpret_cast<sockaddr*>(&SockAddr), sizeof(SockAddr));
    if (BindResult == SOCKET_ERROR)
    {
        cerr << "Can't bind a socket! Quitting" << endl;
        closesocket(ServerSocket);
        WSACleanup();
        return -3;
    }

    int32 ListenResult = listen(ServerSocket, SOMAXCONN);
    if (ListenResult == SOCKET_ERROR)
    {
        cerr << "Can't listen a socket! Quitting" << endl;
        closesocket(ServerSocket);
        WSACleanup();
        return -4;
    }

    u_long NonBlockingMode = 1;
    ioctlsocket(ServerSocket, FIONBIO, &NonBlockingMode);

    return 1;
}

SOCKET ServerNetwork::Accept()
{
    int32 ClientSize = sizeof(ClientSockInfo);
    return accept(ServerSocket, reinterpret_cast<sockaddr*>(&ClientSockInfo), &ClientSize);
}

void ServerNetwork::AddNewUser(const SOCKET NewSocket)
{
    // 접속 정보라.. 딱히 필요 없는 부분
    //char Host[NI_MAXHOST];
    //char Service[NI_MAXHOST];
    //ZeroMemory(Host, NI_MAXHOST);
    //ZeroMemory(Service, NI_MAXHOST);

    //if (getnameinfo((sockaddr*)&ClientSockInfo, sizeof(ClientSockInfo), Host, NI_MAXHOST, Service, NI_MAXSERV, 0) == 0)
    //{
    //    cout << Host << " connected ON port " << Service << endl;
    //}
    //else
    //{
    //    inet_ntop(AF_INET, &ClientSockInfo.sin_addr, Host, NI_MAXHOST);
    //    cout << Host << " connected on port " << ntohs(ClientSockInfo.sin_port) << endl;
    //}

    User* NewUser = new User(this, NewSocket);
    UserList.insert({ NewSocket, NewUser });
}

void ServerNetwork::Cleanup()
{
    for (auto& Item : UserList)
    {
        User* CurrentUser = Item.second;
        delete CurrentUser;
    }
    UserList.clear();

    closesocket(ServerSocket);
    ServerSocket = INVALID_SOCKET;
    WSACleanup();
}
