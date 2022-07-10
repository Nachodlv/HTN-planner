#pragma once

// UE Includes
#include "BehaviorTree/BlackboardComponent.h"
#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "NHTNPrimitiveTask.generated.h"

class UNHTNComponent;

UENUM()
enum class ENHTNTaskStatus : uint8
{
	Success,
	InProgress,
	Failed
};

UCLASS(Abstract)
class NHTN_API UNHTNPrimitiveTask : public UObject
{
	GENERATED_BODY()

public:

	/** Runs the operator AI task */
	virtual ENHTNTaskStatus ExecuteTask(const UNHTNComponent& HTNComp) const { return ENHTNTaskStatus::Success; }

	virtual void AbortTask(const UNHTNComponent& HTNComp) const {}
	
	/** Whether this task can be executed in the given world state */
	virtual bool CanBeExecuted(const UBlackboardComponent& WorldState) const { return true; }

	/** Applies the corresponding effects to the world state when the task is executed successfully */
	virtual void ApplyEffects(UBlackboardComponent& WorldState) const {}

	/** Applies the corresponding effects to the world state as if the task was executed. Only used on planning. */
	virtual void ApplyExpectedEffects(UBlackboardComponent& WorldState) const {}

};
