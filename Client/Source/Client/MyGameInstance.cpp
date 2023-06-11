// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Network/ClientNetwork.h"

void UMyGameInstance::Init()
{
	Super::Init();
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UMyGameInstance::Tick));
	NetworkInstance = new FClientNetwork();
	if(NetworkInstance)
		NetworkInstance->Connect();
}

bool UMyGameInstance::Tick(float DeltaSeconds)
{
	if(NetworkInstance)
		NetworkInstance->Tick();
	return true;
}

void UMyGameInstance::Shutdown()
{
	if(NetworkInstance)
		NetworkInstance->Disconnect();
	
	delete NetworkInstance;
	NetworkInstance = nullptr;
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	Super::Shutdown();
}
