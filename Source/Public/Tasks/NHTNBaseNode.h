#pragma once

#include "CoreMinimal.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "UObject/Object.h"
#include "NHTNBaseNode.generated.h"

UCLASS(Abstract, EditInlineNew)
class NHTN_API UNHTNBaseNode : public UObject
{
	GENERATED_BODY()

public:
	/** Whether this task can be executed in the given world state */
	virtual bool CanBeExecuted(const UBlackboardComponent& WorldState) const { return true; }
};
