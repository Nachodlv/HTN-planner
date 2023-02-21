#include "Components/NHTNComponent.h"

// UE Includes
#include "Algo/AllOf.h"
#include "Algo/RemoveIf.h"

#if ENABLE_VISUAL_LOG
#include "VisualLogger/VisualLogger.h"
#endif // ENABLE_VISUAL_LOG

// NHTN Includes
#include "INHTNModule.h"
#include "Components/NHTNBlackboardComponent.h"
#include "Domain/NHTNDomain.h"
#include "Planner/NHTNPlanner.h"
#include "Tasks/NHTNCompoundTask.h"

#if ENABLE_VISUAL_LOG
#include "Types/NHTNTypes.h"
#endif // ENABLE_VISUAL_LOG

UNHTNComponent::UNHTNComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
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
		const TArray<UNHTNBaseTask*>& DomainTasks = DomainPtr->GetTasks();
		TasksInstances.Reset();
		TasksInstances.Reserve(DomainTasks.Num());
		for (const UNHTNBaseTask* DomainTask : DomainTasks)
		{
			TasksInstances.Add(DuplicateObject(DomainTask, this));
		}
		
		ensureMsgf(TasksInstances.Num() != 0, TEXT("No tasks to run"));
		
		for (UNHTNBaseTask* Task : TasksInstances)
		{
			Task->InitializeTask(*this);
		}
		bInitialized = true;
	}
	CurrentTask = INDEX_NONE;
	SetComponentTickEnabled(true);
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
		CurrentTask = INDEX_NONE;
	}
	INHTNModule::Get().GetPlanner()->AbortPlan(CurrentPlanRequest);
	bPlanning = false;
	CurrentTaskStatus = ENHTNTaskStatus::Success;
}

void UNHTNComponent::StopLogic(const FString& Reason)
{
	Super::StopLogic(Reason);
	RestartLogic();
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

	UE_VLOG(GetOwner(), LogNHTN, Log, TEXT("[%s] registering message observer (%s)"),
		*PrimitiveTask->GetTitleDescription(), *Message.ToString());
}

void UNHTNComponent::RegisterMessageObserver(UNHTNPrimitiveTask* PrimitiveTask, const FName& Message,
	const FAIRequestID& InRequestID)
{
	FAIMessageObserverHandle Observer = FAIMessageObserver::Create(this, Message, InRequestID,
		FOnAIMessage::CreateUObject(PrimitiveTask, &UNHTNPrimitiveTask::OnWrappedMessage));
	MessageObservers.Emplace(PrimitiveTask, Message, Observer, InRequestID);

	UE_VLOG(GetOwner(), LogNHTN, Log, TEXT("[%s] registering message observer (%s)"),
		*PrimitiveTask->GetTitleDescription(), *Message.ToString());
}

void UNHTNComponent::UnRegisterMessageObserver(const UNHTNPrimitiveTask* PrimitiveTask, const FName& Message,
	const FAIRequestID& InRequestID)
{
	MessageObservers.RemoveSwap(FNHTNMessageObserver(PrimitiveTask, Message, InRequestID));

	UE_VLOG(GetOwner(), LogNHTN, Log, TEXT("[%s] unregistering message observer (%s)"),
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
	UE_VLOG(GetOwner(), LogNHTN, Log, TEXT("Message received (%s)"), *Message.MessageName.ToString());
	SetComponentTickEnabled(true);
}

void UNHTNComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bNeedsTicking = false;
	if (!bPaused)
	{
		bool bNeedsPlanning = Plan.Num() == 0 || CurrentTaskStatus == ENHTNTaskStatus::Failed;
		
		if (!bNeedsPlanning && (CurrentTask == INDEX_NONE || CurrentTaskStatus != ENHTNTaskStatus::InProgress))
		{
			++CurrentTask;
			if (Plan.IsValidIndex(CurrentTask) && Plan[CurrentTask]->CanBeExecuted(*GetHTNBBComp()))
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
	bPlanning = true;

	Plan.Reset();

	UNHTNPlanner* Planner = INHTNModule::Get().GetPlanner();
	FNHTNPlanRequest Request;
	Request.TasksInstances = TasksInstances;
	Request.NHTNComponent = this;
	Request.Delegate.BindUObject(this, &UNHTNComponent::PlanFinished);
	CurrentPlanRequest = Planner->GeneratePlan(MoveTemp(Request));
	
	UE_VLOG_UELOG(GetOwner(), LogNHTN, Log, TEXT("Plan requested with index: %d"), CurrentPlanRequest);
}

void UNHTNComponent::PlanFinished(FNHTNPlanResult Result)
{
	Algo::Transform(Result.Plan, Plan,
		[](const TWeakObjectPtr<UNHTNPrimitiveTask>& Task) { return Task.Get(); });
	CurrentTaskStatus = ENHTNTaskStatus::Success;
	UE_VLOG_UELOG(GetOwner(), LogNHTN, Log, TEXT("New Plan created containing %d tasks"), Plan.Num());
	bPlanning = false;
}

UNHTNBlackboardComponent* UNHTNComponent::GetHTNBBComp()
{
	return Cast<UNHTNBlackboardComponent>(GetBlackboardComponent());
}

const UNHTNBlackboardComponent* UNHTNComponent::GetHTNBBComp() const
{
	return Cast<UNHTNBlackboardComponent>(GetBlackboardComponent());
}

void UNHTNComponent::DescribeSelfToVisLog(FVisualLogEntry* Snapshot) const
{
	Super::DescribeSelfToVisLog(Snapshot);
	if (IsRunning() && !bPlanning)
	{
		const UNHTNBlackboardComponent& WorldState = *GetHTNBBComp();
		FVisualLogStatusCategory& NHTNComponentCategory = Snapshot->Status.Emplace_GetRef(TEXT("NHTN Component"));
		const UNHTNPrimitiveTask* RunningTask = GetCurrentTask();
		const FString DebugTask = FString::Printf(TEXT("%s, %s"), *RunningTask->GetTitleDescription(),
			*RunningTask->GetRuntimeDescription(WorldState));
		NHTNComponentCategory.Add(FString(TEXT("Running")), DebugTask);
	}

	if (Plan.Num() > 0)
	{
		FVisualLogStatusCategory& PlanCategory = Snapshot->Status.Emplace_GetRef(TEXT("Current Plan"));
		for (int32 i = 0; i < Plan.Num(); ++i)
		{
			const FString Index = FString::Printf(TEXT("%s%d"), CurrentTask == i ? TEXT("*") : TEXT(""), i); 
			PlanCategory.Add(Index, *Plan[i]->GetTitleDescription());
		}
	}
}

bool UNHTNComponent::IsRunning() const
{
	return Plan.IsValidIndex(CurrentTask);
}

void UNHTNComponent::SetCurrentTaskStatus(ENHTNTaskStatus NewStatus)
{
	const UNHTNPrimitiveTask* CurrentTaskNode = GetCurrentTask();
	ENHTNTaskStatus OldStatus = CurrentTaskStatus;
	
	if (NewStatus == ENHTNTaskStatus::Success && IsRunning())
	{
		CurrentTaskNode->ApplyEffects(*GetBlackboardComponent());
	}
	CurrentTaskStatus = NewStatus;
	SetComponentTickEnabled(true);
	
	UE_VLOG(GetOwner(), LogNHTN, Log, TEXT("[%s] changing status from (%s) to (%s)"),
		*GetNameSafe(CurrentTaskNode),
		*NHTN_ENUM_TO_STRING(ENHTNTaskStatus, OldStatus), *NHTN_ENUM_TO_STRING(ENHTNTaskStatus, NewStatus));
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
		UE_VLOG(OwnerActor, LogNHTN, Log, TEXT("[%s] unregistering message observer (%s)"),
			MessageObserver.Task.IsValid() ? *MessageObserver.Task->GetTitleDescription() : TEXT("None"),
			*MessageObserver.Message.ToString());
		return MessageObserver.Task == Task;
	});
}




