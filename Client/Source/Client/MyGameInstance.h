// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Network/Network.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UMyGameInstance : public UGameInstance, public Network
{
	GENERATED_BODY()
public:
	UMyGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;
	bool IsConnected() const;
	int32 SendPacket(Packet::Header* InPacket);

protected: // socket 값은 ClientSocket 와 동일하므로 필요 없다. 
	virtual void OnConnected(const uint64) override;
	virtual void OnDisconnected(const uint64) override;
	virtual void OnRpLogin(const uint64, const Packet::RpLogin* InPacket) override;
	virtual void OnNtSpawn(const uint64 InSocket, const Packet::NtSpawn* InPacket) override;
	virtual void OnRpHeartbeat(const uint64, const Packet::RpHeartbeat* InPacket) override;
	virtual void OnNtMove(const uint64, const Packet::NtMove* InPacket) override;

private:
	int Connect();
	bool Tick(float DeltaSeconds);
	void Disconnect();
	
	void Cleanup();

	FTSTicker::FDelegateHandle TickDelegateHandle;
	uint64 ClientSocket;
	float MovementSentTimerTime = 0.0f;

	UPROPERTY()
	APlayerController* PlayerController = nullptr;
	UPROPERTY()
	class ALocalCharacter* LocalPlayer = nullptr;
	UPROPERTY()
	TMap<uint64, TScriptInterface<class IServerSpawnable>> PlayerMap;
};
