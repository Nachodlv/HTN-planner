#pragma once

// UE Includes
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "CoreMinimal.h"
#include "UObject/Object.h"

// NHTN Includes
#include "Components/NHTNComponent.h"

#include "NHTNKeyObserver.generated.h"

UENUM(BlueprintType)
enum class ENHTNObservedKeyReplan : uint8
{
	/** Aborts the current running task and makes a new plan */
	AbortTask,
	/** Waits for the current task to finish and makes a new plan */
	WaitForTask
};

/** Observes a blackboard key for changes and notifies the HTN component when it does */
UCLASS(EditInlineNew)
class NHTN_API UNHTNKeyObserver : public UObject
{
	GENERATED_BODY()

public:
	/** Registers to the BlackboardKey change */
	virtual void Initialize(UNHTNComponent* InHTNComp);

protected:
	/** Unregisters to the BlackboardKey change */
	virtual void BeginDestroy() override;

	/** Notifies the key change */
	virtual EBlackboardNotificationResult OnKeyObservedChange(const UBlackboardComponent& BlackboardComponent, uint8 KeyID);

	/** Returns the BlackboardKey value from the blackboard component */
	template<class TDataClass>
	typename TDataClass::FDataType GetCurrentValue()
	{
		return HTNComp->GetBlackboardComponent()->GetValue<TDataClass>(BlackboardKey.SelectedKeyName);
	}

	/** Resets the current plan */
	void NotifyKeyChange() const;

	/** The key that will be observed */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	FBlackboardKeySelector BlackboardKey;
	
	UPROPERTY(Transient)
	TObjectPtr<UNHTNComponent> HTNComp = nullptr;
	
private:
	/** How the observer will handle the key change */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	ENHTNObservedKeyReplan ReplanType;
};
