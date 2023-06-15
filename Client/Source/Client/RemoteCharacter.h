// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ServerSpawnable.h"
#include "GameFramework/Character.h"
#include "RemoteCharacter.generated.h"

// https://unreal-mmo-dev.com/2021/10/28/11-create-mmo-server-handle-motion/
// 스폰 패킷 struct FProxyCharacter
// 이동 패킷 struct FMotion

UCLASS()
class CLIENT_API ARemoteCharacter : public ACharacter, public IServerSpawnable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARemoteCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NtMove(const Packet::NtMove* InPacket, const uint64 ServerTime) override;
	virtual void Leave() override;
	

private:
	UPROPERTY()
	TObjectPtr<class URemoteMovementComponent> RemoteMovement;
};
