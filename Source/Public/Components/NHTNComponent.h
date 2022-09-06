#pragma once

// UE Includes
#include "BrainComponent.h"
#include "CoreMinimal.h"

// NHTN Includes
#include "Tasks/NHTNPrimitiveTask.h"

#include "NHTNComponent.generated.h"

class AAIController;

class UNHTNDomain;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NHTN_API UNHTNComponent : public UBrainComponent
{
	GENERATED_BODY()

public:
	UNHTNComponent();

	// ~ Begin UBrainComponent
	virtual void StartLogic() override;
	virtual void RestartLogic() override;
	virtual void StopLogic(const FString& Reason) override;
	virtual void PauseLogic(const FString& Reason) override;
	virtual EAILogicResuming::Type ResumeLogic(const FString& Reason) override;
	// ~ End UBrainComponent

	/** Sets the domain that will be run when the HTN is executed */
	void SetDomain(const TSoftObjectPtr<UNHTNDomain>& HTNDomain) { Domain = HTNDomain; }

protected:
	// ~ Begin UBrainComponent
	virtual bool IsRunning() const override { return bRunning; }
	virtual bool IsPaused() const override { return bPaused; }
	virtual void Cleanup() override;
	virtual void HandleMessage(const FAIMessage& Message) override;
	// ~ End UBrainComponent

	/** Runs the corresponding task */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// TODO (Ignacio) move to a manager or something
	/** Creates a new plan based on the Domain assigned */
	void StartPlanning();

private:
	/** Contains the tasks used to run the HTN */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	TSoftObjectPtr<UNHTNDomain> Domain;

	/** The current plan that is being executed */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UNHTNBaseTask>> Plan;

	/** Whether the tasks from the domain where initialized */
	bool bInitialized = false;

	bool bRunning = false;
	bool bPaused = false;

	/** The current task index from the InitializedTasks array that is currently running */
	int32 CurrentTask = INDEX_NONE;

	/** The tasks status that is currently running */
	ENHTNTaskStatus CurrentTaskStatus = ENHTNTaskStatus::Success;
};
