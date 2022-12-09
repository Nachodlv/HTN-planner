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
	ExpectedKeysToBeSet = MoveTemp(Other.ExpectedKeysToBeSet);
	Other.ValueMemory.Empty();
	Other.ValueOffsets.Empty();
	ExpectedKeysToBeSet.Empty();
}

FNHTNBlackboardMemory UNHTNBlackboardComponent::RetrieveBBMemory() const
{
	FNHTNBlackboardMemory Memory;
	Memory.ValueMemory = ValueMemory;
	Memory.ValueOffsets = ValueOffsets;
	Memory.ExpectedKeysToBeSet = ExpectedKeysToBeSet;
	return Memory;
}

void UNHTNBlackboardComponent::SetBBMemory(FNHTNBlackboardMemory& InBBMemory)
{
	ValueMemory = MoveTemp(InBBMemory.ValueMemory);
	ValueOffsets = MoveTemp(InBBMemory.ValueOffsets);
	ExpectedKeysToBeSet = MoveTemp(InBBMemory.ExpectedKeysToBeSet);
	InBBMemory.ValueMemory.Reset();
	InBBMemory.ValueOffsets.Reset();
	InBBMemory.ExpectedKeysToBeSet.Reset();
}

bool UNHTNBlackboardComponent::IsKeyExpectedToBeSet(FBlackboard::FKey KeyId) const
{
	return ExpectedKeysToBeSet.Contains(KeyId);
}

bool UNHTNBlackboardComponent::IsKeyExpectedToBeSet(const FName& KeyName) const
{
	return IsKeyExpectedToBeSet(GetKeyID(KeyName));
}

void UNHTNBlackboardComponent::AddExpectedKeyToBeSet(FBlackboard::FKey KeyId)
{
	ExpectedKeysToBeSet.AddUnique(KeyId);
}

void UNHTNBlackboardComponent::AddExpectedKeyToBeSet(const FName& KeyName)
{
	AddExpectedKeyToBeSet(GetKeyID(KeyName));
}

void UNHTNBlackboardComponent::RemoveExpectedKeyToBeSet(FBlackboard::FKey KeyId)
{
	ExpectedKeysToBeSet.Remove(KeyId);
}

void UNHTNBlackboardComponent::RemoveExpectedKeyToBeSet(const FName& KeyName)
{
	RemoveExpectedKeyToBeSet(GetKeyID(KeyName));
}

TOptional<FVector> UNHTNBlackboardComponent::TryGetLocationFromEntry(FBlackboard::FKey KeyId) const
{
	FVector Location = FVector::ZeroVector;
	if (IsKeyExpectedToBeSet(KeyId) || GetLocationFromEntry(KeyId, Location))
	{
		return TOptional(Location);
	}
	return TOptional<FVector>();
}
