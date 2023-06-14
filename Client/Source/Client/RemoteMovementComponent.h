// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RemoteMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API URemoteMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	void RecvMove(const uint64 InServerTime, const FVector& InLocation, const FVector& InDirection, const FVector& InFaceDirection,
		const uint32 InMoveMode, const FVector& InAcceleration, const FVector& InVelocity);
};
