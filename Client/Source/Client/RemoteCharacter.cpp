// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacter.h"
#include "RemoteMovementComponent.h"
#include "Network/Packet.h"


// Sets default values
ARemoteCharacter::ARemoteCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<URemoteMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RemoteMovement = Cast<URemoteMovementComponent>(GetCharacterMovement());
}

// Called when the game starts or when spawned
void ARemoteCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARemoteCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARemoteCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARemoteCharacter::NtMove(const Packet::NtMove* InPacket, const uint64 ServerTime)
{
	SetActorLocationAndRotation(InPacket->Location, InPacket->Direction.ToOrientationRotator());
	RemoteMovement->RecvMove(ServerTime, InPacket->Location, InPacket->Direction, InPacket->FaceDirection, InPacket->MoveMode, InPacket->Acceleration, InPacket->Velocity);
	//InPacket->FaceDirection
}

void ARemoteCharacter::Leave()
{
	SetLifeSpan(0.1f);
}

