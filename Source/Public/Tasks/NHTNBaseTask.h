#pragma once

// UE Includes
#include "CoreMinimal.h"

// NHTN Includes
#include "NHTNBaseNode.h"

#include "NHTNBaseTask.generated.h"

class UNHTNComponent;

UENUM()
enum class ENHTNTaskStatus : uint8
{
	Success,
	InProgress,
	Failed
};

UCLASS(Abstract)
class NHTN_API UNHTNBaseTask : public UNHTNBaseNode
{
	GENERATED_BODY()

public:
	/** Runs the operator AI task */
	virtual ENHTNTaskStatus ExecuteTask(const UNHTNComponent& HTNComp) const { return ENHTNTaskStatus::Success; }

	virtual void AbortTask(const UNHTNComponent& HTNComp) const {}

	virtual void InitializeTask(UNHTNComponent& HTNComp) {}
};
