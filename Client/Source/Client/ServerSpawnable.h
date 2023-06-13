// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ServerSpawnable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UServerSpawnable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CLIENT_API IServerSpawnable
{
	GENERATED_BODY()

public:
public:
	void Initialize(uint64 InUserId);
	uint64 GetUserId() const { return UserId; }

protected:
	uint64		UserId = 0;
};