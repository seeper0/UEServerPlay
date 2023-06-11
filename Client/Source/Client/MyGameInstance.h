// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Chaos/AABB.h>

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	virtual void Shutdown() override;

private:
	bool Tick(float DeltaSeconds);

	FTSTicker::FDelegateHandle TickDelegateHandle;
	class FClientNetwork* NetworkInstance = nullptr;
};
