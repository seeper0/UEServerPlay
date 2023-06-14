// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "LocalCharacter.h"

#undef TEXT
#include <WS2tcpip.h>
#include "HAL/Platform.h" // for TEXT

namespace
{
	constexpr float MOVE_INTERVAL = 0.1f;	
}

DEFINE_LOG_CATEGORY_STATIC(LogClient, Log, All);

UMyGameInstance::UMyGameInstance()
	: ClientSocket(INVALID_SOCKET)
{
}

void UMyGameInstance::Init()
{
	Super::Init();
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UMyGameInstance::Tick));
}

void UMyGameInstance::Shutdown()
{
	Disconnect();
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	Super::Shutdown();
}

bool UMyGameInstance::IsConnected() const
{
	return ClientSocket != INVALID_SOCKET;
}

int UMyGameInstance::Connect()
{
	const char* ServerIpAddr = "127.0.0.1";
	WSADATA WsData;
	const int32 WsResult = WSAStartup(MAKEWORD(2, 2), &WsData);
	if (WsResult != 0)
	{
		UE_LOG(LogClient, Error, TEXT("Can't start Winsock, Err #%d"), WsResult);
		return -1;
	}
	ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket == INVALID_SOCKET)
	{
		const int32 LastError = WSAGetLastError();
		UE_LOG(LogClient, Error, TEXT("Can't create socket, Err #%d"), LastError);
		Cleanup();
		return -2;
	}
	
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(SERVER_PORT);
	const int ConvResult = inet_pton(AF_INET, ServerIpAddr, &hint.sin_addr);
	if (ConvResult != 1)
	{
		UE_LOG(LogClient, Error, TEXT("Can't convert IP address, Err #%d"), ConvResult);
		Cleanup();
		return -3;
	}

	const int ConnResult = connect(ClientSocket, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
	if (ConnResult == SOCKET_ERROR)
	{
		const int32 LastError = WSAGetLastError();
		UE_LOG(LogClient, Error, TEXT("Can't connect to server, Err #%d"), LastError);
		Cleanup();
		return -4;
	}

	u_long NonBlockingMode = 1;
	ioctlsocket(ClientSocket, FIONBIO, &NonBlockingMode);

	OnConnected(ClientSocket);
	return 1;
}

bool UMyGameInstance::Tick(float DeltaSeconds)
{
	if(PlayerController == nullptr)
	{
		PlayerController = GetFirstLocalPlayerController(GetWorld());
	}
	if(LocalPlayer == nullptr && PlayerController != nullptr)
	{
		LocalPlayer = Cast<ALocalCharacter>(PlayerController->GetCharacter());
		if (LocalPlayer != nullptr && ClientSocket == INVALID_SOCKET)
		{
			Connect();
		}
	}

	if(ClientSocket == INVALID_SOCKET || LocalPlayer == nullptr)
		return true;

	const int32 Result = ReceivePacket(ClientSocket);
	if (Result < 0 || Result == WSAEDISCON)
	{
		UE_LOG(LogClient, Error, TEXT("Disconnected, Err #%d"), Result);
		Cleanup();
		return true;
	}

	MovementSentTimerTime += DeltaSeconds;
	if(MovementSentTimerTime > MOVE_INTERVAL)
	{
		Packet::RqMove Packet;
		Packet.Timestamp = GetTickCount64();
		Packet.Location = LocalPlayer->GetActorLocation();
		Packet.Direction = LocalPlayer->GetActorForwardVector();
		Packet.FaceDirection = PlayerController->GetControlRotation().Vector();
		SendPacket(&Packet);
		MovementSentTimerTime = 0;
	}
	
	return true;
}

void UMyGameInstance::Disconnect()
{
	PlayerMap.Empty();
	Cleanup();	
}

int32 UMyGameInstance::SendPacket(Packet::Header* InPacket)
{
	return Network::SendPacket(ClientSocket, InPacket);
}

void UMyGameInstance::Cleanup()
{
	closesocket(ClientSocket);
	WSACleanup();
	ClientSocket = INVALID_SOCKET; 
}

void UMyGameInstance::OnConnected(const uint64)
{
	Packet::RqLogin Request;
	SendPacket(&Request);
}

void UMyGameInstance::OnDisconnected(const uint64)
{
}

void UMyGameInstance::OnRpLogin(const uint64, const Packet::RpLogin* InPacket)
{
	if(LocalPlayer)
	{
		const FRotator Rotator = InPacket->Direction.ToOrientationRotator();
		PlayerController->SetControlRotation(Rotator);
		LocalPlayer->SetActorLocationAndRotation(InPacket->Location, Rotator);
	}
}

void UMyGameInstance::OnNtSpawn(const uint64, const Packet::NtSpawn* InPacket)
{
	const FString Path = TEXT("Class'/Game/ThirdPerson/Blueprints/BP_RemoteCharacter.BP_RemoteCharacter_C'");
	if(UClass* PlayerBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *Path)))
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = nullptr;
		SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map

		const FTransform SpawnTransform(InPacket->Direction.ToOrientationRotator(), InPacket->Location);
		APawn* Pawn = GetWorld()->SpawnActor<APawn>(PlayerBP, SpawnTransform, SpawnInfo);
		if(TScriptInterface<IServerSpawnable> Remote = TScriptInterface<IServerSpawnable>(Pawn))
		{
			Remote->Initialize(InPacket->UserId);
			PlayerMap.Add(InPacket->UserId, Remote);
		}
	}
}

void UMyGameInstance::OnNtLeave(const uint64 InSocket, const Packet::NtLeave* InPacket)
{
	if(TScriptInterface<IServerSpawnable>* Remote = PlayerMap.Find(InPacket->UserId))
	{
		Remote->GetInterface()->Leave();
	}
}

void UMyGameInstance::OnRpHeartbeat(const uint64, const Packet::RpHeartbeat* InPacket)
{
}

void UMyGameInstance::OnNtMove(const uint64, const Packet::NtMove* InPacket)
{
	auto Remote = PlayerMap.Find(InPacket->UserId);
	if(Remote)
	{
		Remote->GetInterface()->NtMove(InPacket);
	}
}
