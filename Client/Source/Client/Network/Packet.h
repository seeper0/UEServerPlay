// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <time.h>
#include <memory.h>

#ifndef __UNREAL__ // only server
struct FVector
{
	FVector(double x, double y, double z) : X(x), Y(y), Z(z) {}
	FVector() : X(0), Y(0), Z(0) {}
	double X, Y, Z;
};

typedef unsigned char 		uint8;
typedef unsigned short int	uint16;
typedef unsigned int		uint32;
typedef unsigned __int64	uint64;
typedef	signed char			int8;
typedef signed short int	int16;
typedef signed int	 		int32;
typedef signed __int64		int64;

#endif

constexpr int MAX_PACKET = 4096;
constexpr int SERVER_PORT = 30777;

#define GENERATED_PACKET(PACKET_NAME)	PACKET_NAME() : Header(EHeaderType::PACKET_NAME, sizeof(PACKET_NAME)) {}

#pragma pack(push, 1)
namespace Packet
{
	enum class EMoveMode : uint8
	{
		None = 0,
		Walking = 1,
		NavWalking = 2,
		Falling = 4,
		Sprinting = 8,
		Swimming = 16
	};
	
	enum class EHeaderType : uint8
	{
		None,
		RqLogin,
		RpLogin,
		NtSpawn,
		NtLeave,
		RqHeartbeat,
		RpHeartbeat,
		RqMove,
		NtMove,
	};

	struct Header
	{
		Header(const EHeaderType InHeader, const int16 InSize) : Type(InHeader), BodySize(InSize - sizeof(Header))
		{}
		EHeaderType Type;
		int16 BodySize = 0;
		int32 GetPacketSize() const { return sizeof(Header) + BodySize; }
	};

	struct RqLogin : Header
	{
		RqLogin() : Header(EHeaderType::RqLogin, sizeof(RqLogin)) {}
	};

	struct RpLogin : Header
	{
		GENERATED_PACKET(RpLogin);

		uint64		UserId = 0;
		FVector		Location;
		FVector		Direction;
	};

	struct NtSpawn : Header
	{
		GENERATED_PACKET(NtSpawn);

		uint64		UserId = 0;
		FVector		Location;
		FVector		Direction;
	};
	
	struct NtLeave : Header
	{
		GENERATED_PACKET(NtLeave);

		uint64		UserId = 0;
	};

	struct RqHeartbeat : Header
	{
		GENERATED_PACKET(RqHeartbeat);
	};

	struct RpHeartbeat : Header
	{
		GENERATED_PACKET(RpHeartbeat);

		uint64		ServerTime = 0;
	};

	struct RqMove : Header
	{
		GENERATED_PACKET(RqMove);

		FVector		Location;
		FVector		Direction;
		FVector		FaceDirection;
		uint32      MoveMode = 0;
		FVector		Acceleration;
		FVector		Velocity;
		uint64		Timestamp = 0;
	};

	struct NtMove : Header
	{
		GENERATED_PACKET(NtMove);

		uint64		UserId = 0;
		FVector		Location;
		FVector		Direction;
		FVector		FaceDirection;
		uint32      MoveMode = 0;
		FVector		Acceleration;
		FVector		Velocity;
		uint64		Timestamp = 0;
	};
}

#pragma pack(pop)

#undef GENERATED_PACKET
