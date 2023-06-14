// Copyright Epic Games, Inc. All Rights Reserved.

#include "Network.h"
#include <chrono>
#undef TEXT
#include <WS2tcpip.h>

#ifndef __UNREAL__ // only server
namespace FMemory
{
	void* Memzero(void* _Dst, size_t _Size)
	{
		return memset(_Dst, 0, _Size);
	}

	void* Memcpy(void* _Dst, void const* _Src, size_t _Size)
	{
		return memcpy(_Dst, _Src, _Size);
	}
}
#endif

using namespace Packet;
namespace
{
	constexpr int MAX_THREAD = 16;
	int NOW_THREAD = 0;
	uint8 PACKET_BUFFER[MAX_THREAD][MAX_PACKET];
}

const Packet::Header* Network::CombinePacket(const Packet::Header* HeaderPacket, char* RawBodyPacket) const
{
	FMemory::Memcpy(PACKET_BUFFER[NOW_THREAD], HeaderPacket, sizeof(Packet::Header));
	FMemory::Memcpy(PACKET_BUFFER[NOW_THREAD] + sizeof(Packet::Header), RawBodyPacket, HeaderPacket->BodySize);
	const Packet::Header* CombinedPacket = reinterpret_cast<Packet::Header*>(PACKET_BUFFER[NOW_THREAD]);
	if (++NOW_THREAD >= MAX_THREAD)
		NOW_THREAD = 0;

	return CombinedPacket;
}


#define CASE_PACKET(IN_SOCKET, PACKET_NAME) case EHeaderType::PACKET_NAME: On##PACKET_NAME(IN_SOCKET, static_cast<const Packet::PACKET_NAME*>(CombinedPacket)); break
void Network::ProcessPacket(const uint64 InSocket, const Packet::Header* HeaderPacket, char* RawBodyPacket)
{
	if (HeaderPacket == nullptr || RawBodyPacket == nullptr)
		return;

	const Packet::Header* CombinedPacket = CombinePacket(HeaderPacket, RawBodyPacket);

	switch (HeaderPacket->Type)
	{
		CASE_PACKET(InSocket, RqLogin);
		CASE_PACKET(InSocket, RpLogin);
		CASE_PACKET(InSocket, NtSpawn);
		CASE_PACKET(InSocket, NtLeave);
		CASE_PACKET(InSocket, RqHeartbeat);
		CASE_PACKET(InSocket, RpHeartbeat);
		CASE_PACKET(InSocket, RqMove);
		CASE_PACKET(InSocket, NtMove);
	}
}
#undef CASE_PACKET

int32 Network::SendPacket(const uint64 InSocket, Packet::Header* InPacket)
{
	unsigned long SendBytes = 0;
	constexpr unsigned long SendFlag = 0;
	WSABUF PacketBuf;
	PacketBuf.len = InPacket->GetPacketSize();
	PacketBuf.buf = const_cast<CHAR*>(reinterpret_cast<const CHAR*>(InPacket));

	WSAOVERLAPPED SendOverlapped;
	SecureZeroMemory((PVOID) & SendOverlapped, sizeof (WSAOVERLAPPED));
	SendOverlapped.hEvent = WSACreateEvent();
	if (SendOverlapped.hEvent == nullptr)
	{
		return -1;
	}
	
	const int SendResult = WSASend(InSocket, &PacketBuf, 1, &SendBytes, SendFlag, &SendOverlapped, nullptr);
	WSAResetEvent(SendOverlapped.hEvent);
	return SendResult;
}

int32 Network::ReceivePacket(const uint64 InSocket)
{
	char PACKET_HEADER[sizeof(Packet::Header)];
	char PACKET_BODY[MAX_PACKET];

	while (true)
	{
		FMemory::Memzero(PACKET_HEADER, sizeof(Packet::Header));
		FMemory::Memzero(PACKET_BODY, MAX_PACKET);

		// Header
		{
			unsigned long HeaderRecved = 0, HeaderFlag = 0;
			WSABUF HeaderBuf;
			WSAOVERLAPPED HeaderOverlapped;
			SecureZeroMemory((PVOID)&HeaderOverlapped, sizeof(WSAOVERLAPPED));
			HeaderBuf.len = sizeof(Packet::Header);
			HeaderBuf.buf = PACKET_HEADER;

			const int HeaderResult = WSARecv(InSocket, &HeaderBuf, 1, &HeaderRecved, &HeaderFlag, nullptr, nullptr);
			const int HeaderError = WSAGetLastError();
			if ((HeaderResult == SOCKET_ERROR) && (HeaderError != WSA_IO_PENDING) && (HeaderError != WSAEWOULDBLOCK))
			{
				return -1;
			}
			if (HeaderError == WSAEWOULDBLOCK || HeaderError == WSA_IO_PENDING)
			{
				return WSAEWOULDBLOCK;
			}
			if (HeaderRecved == 0)
			{
				return WSAEDISCON;
			}
			if (HeaderRecved != sizeof(Packet::Header))
			{
				return -2;
			}
			WSAResetEvent(HeaderOverlapped.hEvent);
		}

		// Body
		Packet::Header* Header = reinterpret_cast<Packet::Header*>(PACKET_HEADER);
		if(Header->BodySize > 0) // 0일 경우 body 없는 정상 패킷
		{
			unsigned long BodyRecved = 0, BodyFlag = 0;
			WSABUF BodyBuf;
			WSAOVERLAPPED BodyOverlapped;
			SecureZeroMemory((PVOID)&BodyOverlapped, sizeof(WSAOVERLAPPED));
			BodyBuf.len = Header->BodySize;
			BodyBuf.buf = PACKET_BODY;
			const int BodyResult = WSARecv(InSocket, &BodyBuf, 1, &BodyRecved, &BodyFlag, nullptr, nullptr);
			const int BodyError = WSAGetLastError();

			if (BodyResult == SOCKET_ERROR)
			{
				return -3;
			}
			if (BodyRecved != Header->BodySize)
			{
				return -4;
			}
			WSAResetEvent(BodyOverlapped.hEvent);
		}
		ProcessPacket(InSocket, Header, PACKET_BODY);
	}
	// what error?
	return -10;
}


uint64 Network::GetMilliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}


