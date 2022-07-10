#pragma once

// UE Includes
#include "CoreMinimal.h"

#include "NHTNDomain.generated.h"

class UNHTNPrimitiveTask;

UCLASS()
class NHTN_API UNHTNDomain : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	const TArray<TSoftClassPtr<UNHTNPrimitiveTask>>& GetTasks() const { return Tasks; }

private:
	UPROPERTY(EditAnywhere, Category = "NHTN")
	TArray<TSoftClassPtr<UNHTNPrimitiveTask>> Tasks;
};
