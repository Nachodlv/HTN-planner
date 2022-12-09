#pragma once

// UE Includes
#include "CoreMinimal.h"

// NHTN Includes
#include "NHTNBaseNode.h"

#include "NHTNDecorator.generated.h"

// TODO (Ignacio) do we really need this class?
/** A task with a condition. Used for compound tasks */
UCLASS(Abstract)
class NHTN_API UNHTNDecorator : public UNHTNBaseNode
{
	GENERATED_BODY()

public:
	/** Whether the decorator can be executed by the planner. Children should no override this function.
	 * Override UNHTNDecorator::CalculateRawCondition instead */
	bool CanBeExecuted(const UNHTNBlackboardComponent& WorldState) const override;

protected:
	/** The condition that will be check without any modifier from the decorator itself as bInverse */
	virtual bool CalculateRawCondition(const UNHTNBlackboardComponent& WorldState) const { return true; }

	/** Returns whether the result of UNHTNDecorator::CalculateRawCondition will be inversed */
	bool IsConditionInversed() const { return bInverse; }
	
private:
	/** Whether the result of UNHTNDecorator::CalculateRawCondition will be inversed */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	bool bInverse = false;
};
