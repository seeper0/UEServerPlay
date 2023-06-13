// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <time.h>

#ifndef __UNREAL__ // only server
struct FVector
{
	FVector() : X(0), Y(0), Z(0)
	{}
	double X, Y, Z;
};
#endif

#pragma pack(push, 1)

namespace Packet
{
	enum class HeaderType : uint8
	{
		RqLogin,
		RpLogin,
		RqHeatbeat,
		RpHeatbeat,
		RqMove,
		NtMove
	};
	
	struct Header
	{
		Header(const HeaderType InHeader) : HeaderCode(InHeader)
		{}
		HeaderType HeaderCode;
	};

	struct RqLogin : Header
	{
		RqLogin() : Header(HeaderType::RqLogin) {}
		FVector		Location;
		FVector		Direction;
	};

	struct RpLogin : Header
	{
		RpLogin() : Header(HeaderType::RpLogin) {}
		int32		UserID = 0;
		FVector		Location;
		FVector		Direction;
	};

	struct RqHeatbeat : Header
	{
		RqHeatbeat() : Header(HeaderType::RqHeatbeat) {}
	};

	struct RpHeatbeat : Header
	{
		RpHeatbeat() : Header(HeaderType::RpHeatbeat) {}
		uint64		Timestamp = 0;
	};

	struct RqMove : Header
	{
		RqMove() : Header(HeaderType::RqMove) {}
		FVector		Location;
		FVector		Direction;
		uint64		Timestamp = 0;
	};

	struct NtMove : Header
	{
		NtMove() : Header(HeaderType::NtMove) {}
		int32		UserID;
		FVector		Location;
		FVector		Direction;
		uint64_t	Timestamp = 0;
	};
}

#pragma pack(pop)
