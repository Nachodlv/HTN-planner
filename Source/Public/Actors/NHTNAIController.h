#pragma once

// UE Includes
#include "CoreMinimal.h"
#include "AIController.h"

#include "NHTNAIController.generated.h"

class UNHTNComponent;
class UNHTNDomain;

/** Basic AIController that provides the functionality to run a HTN */
UCLASS()
class NHTN_API ANHTNAIController : public AAIController
{
	GENERATED_BODY()

protected:
	/** Initializes the HTNComponent and runs it */
	void RunHTN();

private:
	/** Used to the run HTN Component when calling RunHTN */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	TSoftObjectPtr<UNHTNDomain> Domain;

	/** Component currently being used to run the Domain */
	UPROPERTY(Transient)
	TObjectPtr<UNHTNComponent> HTNComponent = nullptr;
};
