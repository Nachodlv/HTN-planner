#pragma once

// UE Includes
#include "CoreMinimal.h"

#include "NHTNDomain.generated.h"

class UNHTNBaseTask;

UCLASS()
class NHTN_API UNHTNDomain : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	const TArray<UNHTNBaseTask*>& GetTasks() const { return Tasks; }

private:
	UPROPERTY(EditAnywhere, Instanced, Category = "NHTN")
	TArray<TObjectPtr<UNHTNBaseTask>> Tasks;
};
