#pragma once

#include "CoreMinimal.h"

// NHTHN Includes
#include "NHTNBaseTask.h"

#include "NHTNCompoundTask.generated.h"

class UNHTNDecorator;

USTRUCT()
struct FNHTNMethod
{
	GENERATED_BODY()

	/** The conditions to execute the tasks */
	UPROPERTY(EditAnywhere, Instanced)
	TArray<TObjectPtr<UNHTNDecorator>> Decorators;

	UPROPERTY(EditAnywhere, Instanced)
	TArray<TObjectPtr<UNHTNBaseTask>> Tasks;
};

/** Task that contains multiples Tasks with a shared condition */
UCLASS()
class NHTN_API UNHTNCompoundTask : public UNHTNBaseTask
{
	GENERATED_BODY()

public:
	virtual void InitializeTask(UNHTNComponent& HTNComp) override;

	const TArray<FNHTNMethod>& GetMethods() const { return Methods; }

private:
	/** Contains the conditions and the tasks to execute */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	TArray<FNHTNMethod> Methods;
};
