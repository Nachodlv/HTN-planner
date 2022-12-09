#pragma once

#include "CoreMinimal.h"

#include "Tasks/NHTNDecorator.h"

#include "NHTNIsBBKeySetDecorator.generated.h"

/** Checks whether a blackboard is set */
UCLASS()
class NHTN_API UNHTNIsBBKeySetDecorator : public UNHTNDecorator
{
	GENERATED_BODY()

protected:
	/** Returns whether the BBKey is set or is expected to be set */
	virtual bool CalculateRawCondition(const UNHTNBlackboardComponent& WorldState) const override;
	
private:
	/** The key that will be checked if it is set */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	FBlackboardKeySelector BBKey;
};
