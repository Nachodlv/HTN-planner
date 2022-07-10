#include "Components/NHTNComponent.h"

// NHTN Includes
#include "Tasks/NHTNDomain.h"

UNHTNComponent::UNHTNComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNHTNComponent::StartLogic()
{
	if (!bInitialized)
	{
		if (!ensureMsgf(Domain, TEXT("No HTN domain provided")))
		{
			return;
		}
		const UNHTNDomain* DomainPtr = Domain.LoadSynchronous();
		const TArray<TSoftClassPtr<UNHTNPrimitiveTask>>& Tasks = DomainPtr->GetTasks();

		for (const TSoftClassPtr<UNHTNPrimitiveTask>& TaskClass : Tasks)
		{
			InitializedTasks.Add(NewObject<UNHTNPrimitiveTask>(this, TaskClass.LoadSynchronous()));
		}
		bInitialized = true;
	}
	if (!ensureMsgf(InitializedTasks.Num() != 0, TEXT("No tasks to run")))
	{
		return;
	}
	bRunning = true;
	CurrentTask = INDEX_NONE;
}

void UNHTNComponent::RestartLogic()
{
	Super::RestartLogic();
	if (IsRunning())
	{
		CurrentTask = INDEX_NONE;
	}
}

void UNHTNComponent::StopLogic(const FString& Reason)
{
	Super::StopLogic(Reason);
	RestartLogic();
	bRunning = false;
	bPaused = false;
}

void UNHTNComponent::PauseLogic(const FString& Reason)
{
	Super::PauseLogic(Reason);
	bPaused = true;
}

EAILogicResuming::Type UNHTNComponent::ResumeLogic(const FString& Reason)
{
	if (!IsPaused())
	{
		StartLogic();
		return EAILogicResuming::RestartedInstead;
	}
	bRunning = true;
	return EAILogicResuming::Continue;
}

void UNHTNComponent::Cleanup()
{
	StopLogic(FString(TEXT("Cleanup")));

	InitializedTasks.Reset();
	bInitialized = false;
}

void UNHTNComponent::HandleMessage(const FAIMessage& Message)
{
	Super::HandleMessage(Message);
	// TODO (Ignacio) handle messages
	ensureMsgf(false, TEXT("Not handling messages yet"));
}

void UNHTNComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bRunning && !bPaused)
	{
		if (CurrentTask == INDEX_NONE || CurrentTaskStatus != ENHTNTaskStatus::InProgress)
		{
			CurrentTask = (CurrentTask + 1) % InitializedTasks.Num();
			CurrentTaskStatus = InitializedTasks[CurrentTask]->ExecuteTask(*this);
		}
	}
}




