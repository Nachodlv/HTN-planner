#pragma once

// UE Includes
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "CoreMinimal.h"
#include "UObject/Object.h"

// NHTN Includes
#include "Components/NHTNComponent.h"
#include "Types/ObjectList/NHTNObjectListerContainer.h"

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
class NHTN_API UNHTNKeyObserver : public UObject, public INHTNObjectListerContainer
{
	GENERATED_BODY()

public:
	/** Registers to the BlackboardKey change */
	virtual void Initialize(UNHTNComponent* InHTNComp);

#if WITH_EDITORONLY_DATA
	void SetUpDomain(UNHTNDomain* InDomain) { Domain = InDomain; }
#endif // WITH_EDITORONLY_DATA

protected:
	/** Unregisters to the BlackboardKey change */
	virtual void BeginDestroy() override;

	/** Notifies the key change */
	virtual EBlackboardNotificationResult OnKeyObservedChange(const UBlackboardComponent& BlackboardComponent, uint8 KeyID);

	/** Checks whether the current key observer is relevant */
	UFUNCTION()
	void OnCurrentTaskStateChanged(UNHTNComponent* HtnComponent, UNHTNPrimitiveTask* Task, ENHTNTaskStatus NewStatus);

	/** Starts observing the blackboard key changes */
	virtual void OnBeginRelevance();

	/** Stops observing the blackboard key changes */
	virtual void OnCeaseRelevance();

	/** Returns whether this observer is currently relevant */
	bool IsRelevant() const { return bRelevant; }

	/** Returns the BlackboardKey value from the blackboard component */
	template<class TDataClass>
	typename TDataClass::FDataType GetCurrentValue()
	{
		return HTNComp->GetBlackboardComponent()->GetValue<TDataClass>(BlackboardKey.SelectedKeyName);
	}

	/** Resets the current plan */
	void ResetCurrentPlan() const;

	// ~ Start INHTNObjectListerContainer
	virtual FNHTNObjectList& GetObjectList() override { return RelevantTasks; }

	/** Returns the primitive tasks that will be listed in the RelevantTasks list */
	virtual TArray<UObject*> GetObjectsToList() const override;

	/** How the task will be displayed in the RelevantTask list */
	virtual TTuple<int32, FString> RetrieveNameFromListedObject(int32 ListIndex, const UObject* ListedObject) const override;

	/** Function used to create the selection widget in the editor for the meta property "GetOptions" */
	UFUNCTION()
	virtual TArray<FString> RetrieveObjectNames() const override { return INHTNObjectListerContainer::RetrieveObjectNames(); }
	// ~ End INHTNObjectListerContainer
	
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	/** The key that will be observed */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	FBlackboardKeySelector BlackboardKey;
	
	UPROPERTY(Transient)
	TObjectPtr<UNHTNComponent> HTNComp = nullptr;
	
private:
#if WITH_EDITORONLY_DATA
	/** Used to list the primitive tasks for the RelevantTasks select */
	UPROPERTY()
	TObjectPtr<UNHTNDomain> Domain = nullptr;
#endif // WITH_EDITORONLY_DATA
	
	/** How the observer will handle the key change */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	ENHTNObservedKeyReplan ReplanType;

	/** The tasks this observer will be relevant. If empty it will be relevant during all the plan */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	FNHTNObjectList RelevantTasks;

	/** Whether this observer is currently relevant */
	bool bRelevant = false;
};
