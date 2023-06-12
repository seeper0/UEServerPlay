// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Common/Network.h"

class FClientNetwork final : public Network
{
public:
	FClientNetwork();
	
	int Connect();
	void Tick();
	void Disconnect();
	int32 SendPacket(Packet::Header* InPacket);

protected: // socket 값은 ClientSocket 와 동일하므로 필요 없다. 
	virtual void OnConnected(const uint64) override;
	virtual void OnDisconnected(const uint64) override;
	virtual void OnRpLogin(const uint64, const Packet::RpLogin* InPacket) override;
	virtual void OnNtSpawn(const uint64 InSocket, const Packet::NtSpawn* InPacket) override;
	virtual void OnRpHeartbeat(const uint64, const Packet::RpHeartbeat* InPacket) override;
	virtual void OnNtMove(const uint64, const Packet::NtMove* InPacket) override;

private:
	void Cleanup();
	
	uint64 ClientSocket;
};
