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

protected:
	virtual void RpLogin(const uint64 InSocket, const Packet::RpLogin* Pkt) override;
	virtual void RpHeartbeat(const uint64 InSocket, const Packet::RpHeartbeat* Pkt) override;
	virtual void NtMove(const uint64 InSocket, const Packet::NtMove* Pkt) override;

private:
	void Cleanup();
	
	uint64 ClientSocket;
};
