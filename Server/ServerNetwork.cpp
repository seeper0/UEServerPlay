#include "ServerNetwork.h"
#include <iostream>
#include <list>
#include "User.h"

using std::cout;
using std::cerr;
using std::endl;

//https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/
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
        DWORD WaitResult = WSAWaitForMultipleEvents(EventList.size(), EventList.data(), FALSE, WSA_INFINITE, FALSE);
        DWORD Index = WaitResult - WSA_WAIT_EVENT_0;
        BOOL bResult = WSAResetEvent(EventList[Index]);
        if (EventList[Index] == AcceptHandel)
        {
            Accept();
        }
        else
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
                WSAEVENT RecvHandle = Item->GetRecvHandel();
                delete Item;
                UserList.erase(Socket);

                auto it = std::find(EventList.begin(), EventList.end(), RecvHandle);
                if (it != EventList.end())
                {
                    EventList.erase(it);
                }
            }
        }
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

    //u_long NonBlockingMode = 1;
    //ioctlsocket(ListenSocket, FIONBIO, &NonBlockingMode);

    AcceptHandel = WSACreateEvent();
    WSAEventSelect(ListenSocket, AcceptHandel, FD_ACCEPT);
    EventList.push_back(AcceptHandel);

    return 1;
}

SOCKET ServerNetwork::Accept()
{
    sockaddr_in ClientSockInfo;
    int32 ClientSize = sizeof(ClientSockInfo);
    SOCKET Socket = WSAAccept(ListenSocket, reinterpret_cast<SOCKADDR*>(&ClientSockInfo), &ClientSize, nullptr, 0 );
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

        WSAEVENT RecvHandel = WSACreateEvent();
        WSAEventSelect(Socket, RecvHandel, FD_READ | FD_WRITE | FD_CLOSE);

        User* NewUser = new User(this, Socket, RecvHandel);
        UserList.insert({ Socket, NewUser });
        EventList.push_back(RecvHandel);
        WSAResetEvent(RecvHandel);
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
