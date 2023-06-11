// Copyright Epic Games, Inc. All Rights Reserved.

#include "Network.h"
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


#define CASE_PACKET(IN_SOCKET, PACKET_NAME) case HeaderType::PACKET_NAME: PACKET_NAME(IN_SOCKET, static_cast<const Packet::PACKET_NAME*>(CombinedPacket)); break;
void Network::ProcessPacket(const uint64 InSocket, const Packet::Header* HeaderPacket, char* RawBodyPacket)
{
	if (HeaderPacket == nullptr || RawBodyPacket == nullptr)
		return;

	const Packet::Header* CombinedPacket = CombinePacket(HeaderPacket, RawBodyPacket);

	switch (HeaderPacket->Type)
	{
		CASE_PACKET(InSocket, RqLogin)
			CASE_PACKET(InSocket, RpLogin)
			CASE_PACKET(InSocket, RqHeartbeat)
			CASE_PACKET(InSocket, RpHeartbeat)
			CASE_PACKET(InSocket, RqMove)
			CASE_PACKET(InSocket, NtMove)
	}
}
#undef CASE_PACKET

int32 Network::ReceivePacket(const uint64 InSocket)
{
	char PACKET_HEADER[sizeof(Packet::Header)];
	char PACKET_BODY[MAX_PACKET];

	while (true)
	{
		FMemory::Memzero(PACKET_HEADER, sizeof(Packet::Header));
		FMemory::Memzero(PACKET_BODY, MAX_PACKET);

		int32 HeaderSize = recv(InSocket, PACKET_HEADER, sizeof(Packet::Header), 0);
		if (HeaderSize == sizeof(Packet::Header))
		{
			Packet::Header* Header = reinterpret_cast<Packet::Header*>(PACKET_HEADER);
			int32 BodySize = recv(InSocket, PACKET_BODY, Header->BodySize, 0);
			if (BodySize == Header->BodySize)
			{
				ProcessPacket(InSocket, Header, PACKET_BODY);
			}
			else
			{
				// body error
				return -2;
			}
		}
		else if (HeaderSize == 0)
		{
			// 큐가 비었다면 정상이다.
			return 1;
		}
		else
		{
			// header error
			return -1;
		}
	}
	// what error?
	return -3;
}

