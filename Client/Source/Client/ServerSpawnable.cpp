// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSpawnable.h"

// Add default functionality here for any IServerSpawnable functions that are not pure virtual.
void IServerSpawnable::Initialize(uint64 InUserId)
{
	UserId = InUserId;
}
