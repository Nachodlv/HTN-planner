#pragma once

// UE Includes
#include "CoreMinimal.h"

// NHTN Includes
#include "Tasks/NHTNPrimitiveTask.h"

#include "NHTNWaitTask.generated.h"

/** Stays in progress for a given time */
UCLASS()
class NHTN_API UNHTNWaitTask : public UNHTNPrimitiveTask
{
	GENERATED_BODY()

public:
	virtual ENHTNTaskStatus ExecuteTask(UNHTNComponent& HTNComp) override;

	virtual void AbortTask(UNHTNComponent& HTNComp) override;

protected:
	void OnWaitTimeEnded();
	
private:
	/** The time the task will stay in progress before finishing */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	float TimeToWait = 1.0f;

	/** How much the TimeToWait can deviate */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	float TimeDeviation = 0.0f;

	UPROPERTY(Transient)
	TObjectPtr<UNHTNComponent> NHTNComponent = nullptr;

	FTimerHandle TimerHandle;
};
