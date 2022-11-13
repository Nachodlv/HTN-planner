#pragma once

// UE Includes
#include "BrainComponent.h"
#include "CoreMinimal.h"

// NHTN Includes
#include "NHTNBlackboardComponent.h"
#include "Tasks/NHTNPrimitiveTask.h"

#include "NHTNComponent.generated.h"

class AAIController;

class UNHTNDomain;

/** Contains the information regarding a task subscribed to a message */
struct FNHTNMessageObserver
{
	FNHTNMessageObserver(const UNHTNPrimitiveTask* InTask, const FName& InMessage,
		const FAIMessageObserverHandle& InObserverHandle, const FAIRequestID& InRequestID = FAIRequestID::AnyRequest)
		: Task(InTask), Message(InMessage), ObserverHandle(InObserverHandle), RequestID(InRequestID) {}

	FNHTNMessageObserver(const UNHTNPrimitiveTask* InTask, const FName& InMessage,
		const FAIRequestID& InRequestID = FAIRequestID::AnyRequest)
		: Task(InTask), Message(InMessage), RequestID(InRequestID) {}

	/** The task that made the subscription */
	TWeakObjectPtr<const UNHTNPrimitiveTask> Task;

	/** The message name subscribed to */
	FName Message;

	/** The subscription handle. If destroyed, the subscription will be unregistered */
	FAIMessageObserverHandle ObserverHandle;

	FAIRequestID RequestID;

	friend bool operator==(const FNHTNMessageObserver& Rhs, const FNHTNMessageObserver& Lhs)
	{
		return Rhs.Task == Lhs.Task && Rhs.Message == Lhs.Message && Rhs.RequestID == Lhs.RequestID;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NHTN_API UNHTNComponent : public UBrainComponent
{
	GENERATED_BODY()

#if WITH_GAMEPLAY_DEBUGGER
	friend class FNHTNGameplayDebugger_HTN;
#endif // WITH_GAMEPLAY_DEBUGGER
	
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

	/** Changes the status of the current running task */
	void FinishLatentTask(ENHTNTaskStatus Status);

	/** Registers the task to the given Message. Will be automatically unregistered when the task finishes running */
	void RegisterMessageObserver(UNHTNPrimitiveTask* PrimitiveTask, const FName& Message);
	void RegisterMessageObserver(UNHTNPrimitiveTask* PrimitiveTask, const FName& Message,
		const FAIRequestID& InRequestID);

	/** Unregisters the task from a previous registered message */
	void UnRegisterMessageObserver(const UNHTNPrimitiveTask* PrimitiveTask, const FName& Message,
		const FAIRequestID& InRequestID = FAIRequestID::AnyRequest);

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

	UFUNCTION(BlueprintPure)
	UNHTNBlackboardComponent* GetHTNBBComp();

	/** Sets the status of the current running tasks. Executes its effects if necessary */
	void SetCurrentTaskStatus(ENHTNTaskStatus NewStatus);

	/** Returns the current running task */
	UNHTNPrimitiveTask* GetCurrentTask() const;

	/** Removes all the message observers of the given task */
	void RemoveTaskMessageObservers(int32 TaskIndex);

private:
	/** Contains the tasks used to run the HTN */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	TSoftObjectPtr<UNHTNDomain> Domain;

	/** The current plan that is being executed */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UNHTNPrimitiveTask>> Plan;

	/** The message observers from the current plan's tasks */
	TArray<FNHTNMessageObserver> MessageObservers;

	/** Whether the tasks from the domain where initialized */
	bool bInitialized = false;

	bool bRunning = false;
	bool bPaused = false;
	bool bPlanning = false;

	/** The current task index from the InitializedTasks array that is currently running */
	int32 CurrentTask = INDEX_NONE;

	/** The tasks status that is currently running */
	ENHTNTaskStatus CurrentTaskStatus = ENHTNTaskStatus::Success;
};
