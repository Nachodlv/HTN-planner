#pragma once

// UE Includes
#include "CoreMinimal.h"
#include "UObject/Object.h"

// NHTN Includes
#include "Components/NHTNBlackboardComponent.h"

#include "NHTNBaseNode.generated.h"

UCLASS(Abstract, EditInlineNew)
class NHTN_API UNHTNBaseNode : public UObject
{
	GENERATED_BODY()

public:
	UNHTNBaseNode();
	
	/** Whether this task can be executed in the given world state */
	virtual bool CanBeExecuted(const UNHTNBlackboardComponent& WorldState) const { return true; }

	virtual FString GetTitleDescription() const { return Title; }

#if WITH_GAMEPLAY_DEBUGGER
	virtual FString GetRuntimeDescription(const UNHTNBlackboardComponent& WorldState) const { return FString(); }
#endif // WITH_GAMEPLAY_DEBUGGER

private:
	/** Debug title for the node */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	FString Title;
};
