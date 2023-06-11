// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Packet.h"

class Network
{
public:
	int32 ReceivePacket(const uint64 Socket);
	void ProcessPacket(const uint64 InSocket, const Packet::Header* HeaderPacket, char* RawBodyPacket);

private:
	const Packet::Header* CombinePacket(const Packet::Header* HeaderPacket, char* RawBodyPacket) const;
	
protected:
	virtual void RqLogin(const uint64 InSocket, const Packet::RqLogin* Pkt) {}
	virtual void RpLogin(const uint64 InSocket, const Packet::RpLogin* Pkt) {}
	virtual void RqHeartbeat(const uint64 InSocket, const Packet::RqHeartbeat* Pkt) {}
	virtual void RpHeartbeat(const uint64 InSocket, const Packet::RpHeartbeat* Pkt) {}
	virtual void RqMove(const uint64 InSocket, const Packet::RqMove* Pkt) {}
	virtual void NtMove(const uint64 InSocket, const Packet::NtMove* Pkt) {}
};
