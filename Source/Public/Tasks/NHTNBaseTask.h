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
	virtual ENHTNTaskStatus ExecuteTask(UNHTNComponent& HTNComp) { return ENHTNTaskStatus::Success; }

	virtual void AbortTask(UNHTNComponent& HTNComp) {}

	virtual void InitializeTask(UNHTNComponent& HTNComp) {}

	UFUNCTION(BlueprintPure)
	UNHTNBaseTask* GetParentTask() const { return ParentTask; }

	UFUNCTION(BlueprintCallable)
	void SetParentTask(UNHTNBaseTask* InParentTask) { ParentTask = InParentTask; }

	/** Returns the unique index given by the domain this task belongs to */
	int32 GetDomainIndex() const { return DomainIndex; }
	
	void SetDomainIndex(int32 InIndex) { DomainIndex = InIndex; }

protected:
	/** The unique index given by the domain this task belongs to */
	UPROPERTY()
	int32 DomainIndex = INDEX_NONE;
	
	/** The task containing this one. Usually a compound task */
	UPROPERTY(Transient)
	TObjectPtr<UNHTNBaseTask> ParentTask = nullptr;
};
