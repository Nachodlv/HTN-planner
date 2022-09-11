﻿#include "Components/NHTNComponent.h"

// UE Includes
#include "Algo/AllOf.h"

// NHTN Includes
#include "Components/NHTNBlackboardComponent.h"
#include "Domain/NHTNDomain.h"
#include "Tasks/NHTNCompoundTask.h"

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
	if (IsRunning())
	{
		if (Plan.Num() > 0)
		{
			Plan[CurrentTask]->AbortTask(*this);
		}
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
		Plan[CurrentTask]->AbortTask(*this);
	}
	SetCurrentTaskStatus(Status);
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
	// TODO (Ignacio) handle messages
	ensureMsgf(false, TEXT("Not handling messages yet"));
}

void UNHTNComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bRunning && !bPaused)
	{
		bool bNeedsPlanning = Plan.Num() == 0;
		
		if (!bNeedsPlanning && (CurrentTask == INDEX_NONE || CurrentTaskStatus != ENHTNTaskStatus::InProgress))
		{
			++CurrentTask;
			if (Plan.IsValidIndex(CurrentTask))
			{
				SetCurrentTaskStatus(Plan[CurrentTask]->ExecuteTask(*this));
			}
			else
			{
				bNeedsPlanning = true;
			}
		}

		if (bNeedsPlanning)
		{
			StartPlanning();
			CurrentTask = INDEX_NONE;
		}
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
		Plan[CurrentTask]->ApplyEffects(*GetBlackboardComponent());
	}
	CurrentTaskStatus = NewStatus;
}




