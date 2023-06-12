// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Packet.h"

class Network
{
public:
	int32 SendPacket(const uint64 InSocket, Packet::Header* InPacket);
	int32 ReceivePacket(const uint64 Socket);

private:
	void ProcessPacket(const uint64 InSocket, const Packet::Header* HeaderPacket, char* RawBodyPacket);
	const Packet::Header* CombinePacket(const Packet::Header* HeaderPacket, char* RawBodyPacket) const;
	
protected:
	virtual void OnConnected(const uint64 InSocket) = 0;
	virtual void OnDisconnected(const uint64 InSocket) = 0;
	virtual void OnRqLogin(const uint64 InSocket, const Packet::RqLogin* InPacket) {}
	virtual void OnRpLogin(const uint64 InSocket, const Packet::RpLogin* InPacket) {}
	virtual void OnNtSpawn(const uint64 InSocket, const Packet::NtSpawn* InPacket) {}
	virtual void OnRqHeartbeat(const uint64 InSocket, const Packet::RqHeartbeat* InPacket) {}
	virtual void OnRpHeartbeat(const uint64 InSocket, const Packet::RpHeartbeat* InPacket) {}
	virtual void OnRqMove(const uint64 InSocket, const Packet::RqMove* InPacket) {}
	virtual void OnNtMove(const uint64 InSocket, const Packet::NtMove* InPacket) {}
};
