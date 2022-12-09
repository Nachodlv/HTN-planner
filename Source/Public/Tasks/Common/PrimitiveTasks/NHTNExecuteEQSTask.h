#pragma once

// UE Includes
#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

// NHTN Includes
#include "Tasks/NHTNPrimitiveTask.h"

#include "NHTNExecuteEQSTask.generated.h"

/** Executes an EQS query and saves the result in a blackboard key */
UCLASS()
class NHTN_API UNHTNExecuteEQSTask : public UNHTNPrimitiveTask
{
	GENERATED_BODY()

public:
	UNHTNExecuteEQSTask();

	/** Executed the EQS query */
	virtual ENHTNTaskStatus ExecuteTask(UNHTNComponent& HTNComp) override;

	/** Adds the StoreKey to the expected keys to be set */
	virtual void ApplyExpectedEffects(UNHTNBlackboardComponent& WorldState) const override;

protected:
	/** Stores the results in the StoreKey and finishes the task */
	void OnEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> EnvQueryResult, UNHTNComponent* HTNComp);

	/** Aborts the current EQS query running */
	virtual void AbortTask(UNHTNComponent& HTNComp) override;

private:
	/** The parametrized query to be executed */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	FEQSParametrizedQueryExecutionRequest EQS;

	/** The best item wil be stored in this key */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	FBlackboardKeySelector StoreKey;

	/** Whether the key should be clear of the EQS query fails */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	bool bClearKeyOnFail = false;

	/** The current executing query */
	int32 QueryID = INDEX_NONE;
};
