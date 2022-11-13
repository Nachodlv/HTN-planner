#include "Components/NHTNComponent.h"

// UE Includes
#include "Algo/AllOf.h"
#include "Algo/RemoveIf.h"

#if ENABLE_VISUAL_LOG
#include "VisualLogger/VisualLogger.h"
#endif // ENABLE_VISUAL_LOG

// NHTN Includes
#include "Components/NHTNBlackboardComponent.h"
#include "Domain/NHTNDomain.h"
#include "Tasks/NHTNCompoundTask.h"

#if ENABLE_VISUAL_LOG
#include "Types/NHTNTypes.h"
#endif // ENABLE_VISUAL_LOG

namespace NHTNComponentHelper
{
	template<typename T>
	bool CanAllBeExecuted(const TArray<T>& Nodes, const UBlackboardComponent& WorldState)
	{
		return Algo::AllOf(Nodes, [&WorldState](const T Node)
		{
			return Node->CanBeExecuted(WorldState);
		});
	}

	template<typename T, typename O>
	void PushAllReversed(TArray<T>& Stack, const TArray<O>& Nodes)
	{
		Stack.Reserve(Stack.Num() + Nodes.Num());
		for (int32 i = Nodes.Num() - 1; i >= 0; --i)
		{
			Stack.Push(Nodes[i]);
		}
	}
}

UNHTNComponent::UNHTNComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNHTNComponent::StartLogic()
{
	if (!bInitialized)
	{
		const UNHTNDomain* DomainPtr = Domain.LoadSynchronous();
		if (!ensureMsgf(DomainPtr, TEXT("No HTN domain provided")))
		{
			return;
		}
		const TArray<UNHTNBaseTask*>& Tasks = DomainPtr->GetTasks();
		
		ensureMsgf(Tasks.Num() != 0, TEXT("No tasks to run"));
		
		for (UNHTNBaseTask* Task : Tasks)
		{
			Task->InitializeTask(*this);
		}
		bInitialized = true;
	}
	bRunning = true;
	CurrentTask = INDEX_NONE;
}

void UNHTNComponent::RestartLogic()
{
	Super::RestartLogic();
	if (IsRunning() && Plan.Num() > 0)
	{
		if (!bPlanning)
		{
			GetCurrentTask()->AbortTask(*this);
		}
		MessageObservers.Reset();
		// TODO (Ignacio) we should stop planning if it was async
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

void UNHTNComponent::FinishLatentTask(ENHTNTaskStatus Status)
{
	if (IsRunning())
	{
		GetCurrentTask()->AbortTask(*this);
	}
	RemoveTaskMessageObservers(CurrentTask);
	SetCurrentTaskStatus(Status);
}

void UNHTNComponent::RegisterMessageObserver(UNHTNPrimitiveTask* PrimitiveTask, const FName& Message)
{
	FAIMessageObserverHandle Observer = FAIMessageObserver::Create(this, Message,
		FOnAIMessage::CreateUObject(PrimitiveTask, &UNHTNPrimitiveTask::OnWrappedMessage));
	MessageObservers.Emplace(PrimitiveTask, Message, Observer);

	UE_VLOG(GetOwner(), LogHTN, Log, TEXT("[%s] registering message observer (%s)"),
		*PrimitiveTask->GetTitleDescription(), *Message.ToString());
}

void UNHTNComponent::RegisterMessageObserver(UNHTNPrimitiveTask* PrimitiveTask, const FName& Message,
	const FAIRequestID& InRequestID)
{
	FAIMessageObserverHandle Observer = FAIMessageObserver::Create(this, Message, InRequestID,
		FOnAIMessage::CreateUObject(PrimitiveTask, &UNHTNPrimitiveTask::OnWrappedMessage));
	MessageObservers.Emplace(PrimitiveTask, Message, Observer, InRequestID);

	UE_VLOG(GetOwner(), LogHTN, Log, TEXT("[%s] registering message observer (%s)"),
		*PrimitiveTask->GetTitleDescription(), *Message.ToString());
}

void UNHTNComponent::UnRegisterMessageObserver(const UNHTNPrimitiveTask* PrimitiveTask, const FName& Message,
	const FAIRequestID& InRequestID)
{
	MessageObservers.RemoveSwap(FNHTNMessageObserver(PrimitiveTask, Message, InRequestID));

	UE_VLOG(GetOwner(), LogHTN, Log, TEXT("[%s] unregistering message observer (%s)"),
		*PrimitiveTask->GetTitleDescription(), *Message.ToString());
}

void UNHTNComponent::Cleanup()
{
	StopLogic(FString(TEXT("Cleanup")));

	Plan.Reset();
	bInitialized = false;
}

void UNHTNComponent::HandleMessage(const FAIMessage& Message)
{
	Super::HandleMessage(Message);
	UE_VLOG(GetOwner(), LogHTN, Log, TEXT("Message received (%s)"), *Message.MessageName.ToString());
	SetComponentTickEnabled(true);
}

void UNHTNComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bNeedsTicking = false;
	if (bRunning && !bPaused)
	{
		bool bNeedsPlanning = Plan.Num() == 0;
		
		if (!bNeedsPlanning && (CurrentTask == INDEX_NONE || CurrentTaskStatus != ENHTNTaskStatus::InProgress))
		{
			++CurrentTask;
			if (Plan.IsValidIndex(CurrentTask) && Plan[CurrentTask]->CanBeExecuted(*GetBlackboardComponent()))
			{
				SetCurrentTaskStatus(Plan[CurrentTask]->ExecuteTask(*this));
				bNeedsTicking = CurrentTaskStatus != ENHTNTaskStatus::InProgress;
			}
			else
			{
				bNeedsPlanning = true;
			}
		}

		if (bNeedsPlanning)
		{
			bNeedsTicking = true;
			bPlanning = true;
			StartPlanning();
			CurrentTask = INDEX_NONE;
		}
	}
	if (!bNeedsTicking)
	{
		SetComponentTickEnabled(false);
	}
}

void UNHTNComponent::StartPlanning()
{
	UNHTNBlackboardComponent& BBComp = *GetHTNBBComp();
	// Save the current memory from the blackboard to restore after the planning has finished
	FNHTNBlackboardMemory BBMemory = BBComp.RetrieveBBMemory();
	
	Plan.Reset();
	
	TArray<UNHTNBaseTask*> StackPlan;
	NHTNComponentHelper::PushAllReversed(StackPlan, Domain->GetTasks());
	while (!StackPlan.IsEmpty())
	{
		UNHTNBaseTask* Task = StackPlan.Pop();
		if (const UNHTNCompoundTask* CompoundTask = Cast<UNHTNCompoundTask>(Task))
		{
			const TArray<FNHTNMethod>& Methods = CompoundTask->GetMethods();
			for (const FNHTNMethod& Method : Methods)
			{
				if (NHTNComponentHelper::CanAllBeExecuted(Method.Decorators, BBComp))
				{
					NHTNComponentHelper::PushAllReversed(StackPlan, Method.Tasks);
				}
			}
		}
		else
		{
			UNHTNPrimitiveTask* PrimitiveTask = CastChecked<UNHTNPrimitiveTask>(Task);
			if (PrimitiveTask->CanBeExecuted(BBComp))
			{
				PrimitiveTask->ApplyExpectedEffects(BBComp);
				Plan.Add(PrimitiveTask);
			}
		}
	}

	// Restore memory from blackboard
	BBComp.SetBBMemory(BBMemory);
}

UNHTNBlackboardComponent* UNHTNComponent::GetHTNBBComp()
{
	return Cast<UNHTNBlackboardComponent>(GetBlackboardComponent());
}

void UNHTNComponent::SetCurrentTaskStatus(ENHTNTaskStatus NewStatus)
{
	if (NewStatus == ENHTNTaskStatus::Success && IsRunning())
	{
		GetCurrentTask()->ApplyEffects(*GetBlackboardComponent());
	}
	CurrentTaskStatus = NewStatus;
	SetComponentTickEnabled(true);
}

UNHTNPrimitiveTask* UNHTNComponent::GetCurrentTask() const
{
	return Plan[CurrentTask];
}

void UNHTNComponent::RemoveTaskMessageObservers(int32 TaskIndex)
{
	UNHTNPrimitiveTask* Task = Plan[TaskIndex];
	AActor* OwnerActor = GetOwner();
	Algo::RemoveIf(MessageObservers, [Task, OwnerActor](const FNHTNMessageObserver& MessageObserver)
	{
		UE_VLOG(OwnerActor, LogHTN, Log, TEXT("[%s] unregistering message observer (%s)"),
			MessageObserver.Task.IsValid() ? *MessageObserver.Task->GetTitleDescription() : TEXT("None"),
			*MessageObserver.Message.ToString());
		return MessageObserver.Task == Task;
	});
}




