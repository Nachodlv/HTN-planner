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

	/** We should always prefer moving the moving and copying it */
	FNHTNBlackboardMemory(FNHTNBlackboardMemory&& Other) noexcept { Move(Other); }
	FNHTNBlackboardMemory& operator=(FNHTNBlackboardMemory&& Other) noexcept;

	/** memory block holding all values */
	TArray<uint8> ValueMemory;

	/** offsets in ValueMemory for each key */
	TArray<uint16> ValueOffsets;

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
};
