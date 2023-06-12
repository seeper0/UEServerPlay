// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClientNetwork.h"
#undef TEXT
#include <WS2tcpip.h>
#include "HAL/Platform.h" // for TEXT

DEFINE_LOG_CATEGORY_STATIC(LogClient, Log, All);

FClientNetwork::FClientNetwork()
	: ClientSocket(INVALID_SOCKET)
{
}

int FClientNetwork::Connect()
{
	const char* ServerIpAddr = "127.0.0.1";
	WSADATA WsData;
	const int32 WsResult = WSAStartup(MAKEWORD(2, 2), &WsData);
	if (WsResult != 0)
	{
		UE_LOG(LogClient, Error, TEXT("Can't start Winsock, Err #%d"), WsResult);
		return -1;
	}
	ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket == INVALID_SOCKET)
	{
		const int32 LastError = WSAGetLastError();
		UE_LOG(LogClient, Error, TEXT("Can't create socket, Err #%d"), LastError);
		Cleanup();
		return -2;
	}
	
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(SERVER_PORT);
	const int ConvResult = inet_pton(AF_INET, ServerIpAddr, &hint.sin_addr);
	if (ConvResult != 1)
	{
		UE_LOG(LogClient, Error, TEXT("Can't convert IP address, Err #%d"), ConvResult);
		Cleanup();
		return -3;
	}

	const int ConnResult = connect(ClientSocket, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
	if (ConnResult == SOCKET_ERROR)
	{
		const int32 LastError = WSAGetLastError();
		UE_LOG(LogClient, Error, TEXT("Can't connect to server, Err #%d"), LastError);
		Cleanup();
		return -4;
	}

	u_long NonBlockingMode = 1;
	ioctlsocket(ClientSocket, FIONBIO, &NonBlockingMode);

	OnConnected(ClientSocket);
	return 1;
}

void FClientNetwork::Tick()
{
	const int32 Result = ReceivePacket(ClientSocket);
	if(Result <= 0)
	{
		UE_LOG(LogClient, Error, TEXT("Disconnected, Err #%d"), Result);
		Cleanup();
	}
}

void FClientNetwork::Disconnect()
{
	Cleanup();	
}

int32 FClientNetwork::SendPacket(Packet::Header* InPacket)
{
	return Network::SendPacket(ClientSocket, InPacket);
}

void FClientNetwork::OnConnected(const uint64)
{
	Packet::RqLogin Request;
	SendPacket(&Request);
}

void FClientNetwork::OnDisconnected(const uint64)
{
}

void FClientNetwork::OnRpLogin(const uint64, const Packet::RpLogin* InPacket)
{
}

void FClientNetwork::OnNtSpawn(const uint64 InSocket, const Packet::NtSpawn* InPacket)
{
}

void FClientNetwork::OnRpHeartbeat(const uint64, const Packet::RpHeartbeat* InPacket)
{
}

void FClientNetwork::OnNtMove(const uint64, const Packet::NtMove* InPacket)
{
}

void FClientNetwork::Cleanup()
{
	closesocket(ClientSocket);
	WSACleanup();
	ClientSocket = INVALID_SOCKET; 
}
