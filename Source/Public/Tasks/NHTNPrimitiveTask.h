#pragma once

// UE Includes
#include "BehaviorTree/BlackboardComponent.h"
#include "CoreMinimal.h"
#include "NHTNBaseTask.h"

#include "NHTNPrimitiveTask.generated.h"

class UNHTNComponent;


UCLASS(Abstract)
class NHTN_API UNHTNPrimitiveTask : public UNHTNBaseTask
{
	GENERATED_BODY()

public:
	/** Applies the corresponding effects to the world state when the task is executed successfully */
	virtual void ApplyEffects(UBlackboardComponent& WorldState) const {}

	/** Applies the corresponding effects to the world state as if the task was executed. Only used on planning. */
	virtual void ApplyExpectedEffects(UBlackboardComponent& WorldState) const {}
};
