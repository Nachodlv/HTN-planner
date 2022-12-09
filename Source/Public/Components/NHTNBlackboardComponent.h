#pragma once

// UE Includes
#include "BehaviorTree/BlackboardComponent.h"
#include "CoreMinimal.h"

#include "NHTNBlackboardComponent.generated.h"


struct FNHTNBlackboardMemory
{
	FNHTNBlackboardMemory() {}

	/** Delete copy constructor and copy operator so we don't copy the memory arrays by accident */
	FNHTNBlackboardMemory(const FNHTNBlackboardMemory& Other) = delete;
	FNHTNBlackboardMemory& operator=(const FNHTNBlackboardMemory& Other) = delete;

	/** We should always prefer moving than copying it */
	FNHTNBlackboardMemory(FNHTNBlackboardMemory&& Other) noexcept { Move(Other); }
	FNHTNBlackboardMemory& operator=(FNHTNBlackboardMemory&& Other) noexcept;

	/** memory block holding all values */
	TArray<uint8> ValueMemory;

	/** offsets in ValueMemory for each key */
	TArray<uint16> ValueOffsets;

	/** The expected keys that will be set when executing the plan */
	TArray<FBlackboard::FKey> ExpectedKeysToBeSet;

protected:
	void Move(FNHTNBlackboardMemory& Other);
};

UCLASS()
class NHTN_API UNHTNBlackboardComponent : public UBlackboardComponent
{
	GENERATED_BODY()

public:
	/** Retrieves the key values of the blackboard  */
	FNHTNBlackboardMemory RetrieveBBMemory() const;

	/** Overrides the blackboard key values with the given ones */
	void SetBBMemory(FNHTNBlackboardMemory& InBBMemory);

	/** Returns whether the value going to be set during the execution of the plan */
	bool IsKeyExpectedToBeSet(FBlackboard::FKey KeyId) const;
	bool IsKeyExpectedToBeSet(const FName& KeyName) const;

	/** Adds the key to the expected keys to be set. This will tell the planner that the key will be set when
	 * running the plan */
	void AddExpectedKeyToBeSet(FBlackboard::FKey KeyId);
	void AddExpectedKeyToBeSet(const FName& KeyName);

	/** Removes the key from the expected keys to be set */
	void RemoveExpectedKeyToBeSet(FBlackboard::FKey KeyId);

	/** Returns the location stored in the key if any. Takes into account the expected keys to be set */
	TOptional<FVector> TryGetLocationFromEntry(FBlackboard::FKey KeyId) const;

private:
	/** The expected keys that will be set when executing the plan. This array is used when planning */
	TArray<FBlackboard::FKey> ExpectedKeysToBeSet;
};
