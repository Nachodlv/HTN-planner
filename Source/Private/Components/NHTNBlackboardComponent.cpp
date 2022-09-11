// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/NHTNBlackboardComponent.h"

FNHTNBlackboardMemory& FNHTNBlackboardMemory::operator=(FNHTNBlackboardMemory&& Other) noexcept
{
	if (this != &Other)
	{
		Move(Other);
	}
	return *this;
}

void FNHTNBlackboardMemory::Move(FNHTNBlackboardMemory& Other)
{
	ValueMemory = MoveTemp(Other.ValueMemory);
	ValueOffsets = MoveTemp(Other.ValueOffsets);
	Other.ValueMemory.Empty();
	Other.ValueOffsets.Empty();
}

FNHTNBlackboardMemory UNHTNBlackboardComponent::RetrieveBBMemory() const
{
	FNHTNBlackboardMemory Memory;
	Memory.ValueMemory = ValueMemory;
	Memory.ValueOffsets = ValueOffsets;
	return Memory;
}

void UNHTNBlackboardComponent::SetBBMemory(FNHTNBlackboardMemory& InBBMemory)
{
	ValueMemory = MoveTemp(InBBMemory.ValueMemory);
	ValueOffsets = MoveTemp(InBBMemory.ValueOffsets);
	InBBMemory.ValueMemory.Reset();
	InBBMemory.ValueOffsets.Reset();
}
