#include "Planner/NHTNPlanner.h"

// UE Includes
#include "Algo/AllOf.h"

// NHTN Includes
#include "Components/NHTNComponent.h"
#include "Tasks/NHTNCompoundTask.h"

namespace NHTNPlannerHelper
{
	template<typename T>
	bool CanAllBeExecuted(const TArray<T>& Nodes, const UNHTNBlackboardComponent& WorldState)
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

UNHTNPlanner::UNHTNPlanner()
{
	PlannerClass = UNHTNPlanner::StaticClass();
}

int32 UNHTNPlanner::GeneratePlan(FNHTNPlanRequestParams&& Request)
{
	static int32 RequestID = 0;
	FNHTNPlanRequest& NewRequest = Requests.Emplace_GetRef(MoveTemp(Request));
	NewRequest.UniqueID = ++RequestID;
	return NewRequest.UniqueID;
}

void UNHTNPlanner::AbortPlan(int32 RequestID)
{
	for (int32 i = 0; i < Requests.Num(); ++i)
	{
		if (Requests[i].UniqueID == RequestID)
		{
			Requests.RemoveAt(i);
			break;
		}
	}
}

TStatId UNHTNPlanner::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UNHTNPlanner, STATGROUP_Tickables);
}

bool UNHTNPlanner::IsTickable() const
{
	return Requests.Num() > 0;
}

void UNHTNPlanner::Tick(float DeltaTime)
{
	if (Requests.Num() == 0)
	{
		return;
	}

	float RemainingTime = MaxTimePlanning;
	while (!Requests.IsEmpty())
	{
		const float StartIterationTime = FPlatformTime::Seconds();
		FNHTNPlanRequest& Request = Requests[NextRequestIndex];
		check(Request.PlanResult.State == ENHTNPlanState::Waiting || Request.PlanResult.State == ENHTNPlanState::InProgress);
		if (Request.IsWaitingToBeProcessed())
		{
			NHTNPlannerHelper::PushAllReversed(Request.TasksToVisit, Request.Params.TasksInstances);
			Request.PlanResult.State = ENHTNPlanState::InProgress;
		}
		UNHTNBlackboardComponent& BBComp = *Request.Params.NHTNComponent->GetHTNBBComp();
		FNHTNBlackboardMemory InitialWorldState = BBComp.RetrieveBBMemory();
		MakeOnePlanStep(Request);
		BBComp.SetBBMemory(InitialWorldState);

		if (Request.TasksToVisit.IsEmpty())
		{
			Request.PlanResult.State = Request.PlanResult.Plan.Num() > 0 ? ENHTNPlanState::Finished : ENHTNPlanState::Failed;
			Request.Params.Delegate.ExecuteIfBound(MoveTemp(Request.PlanResult));
			Requests.RemoveAt(NextRequestIndex);
		}
		else if (bPlanUsingBreadth)
		{
			NextRequestIndex = (NextRequestIndex + 1) % Requests.Num();
		}

		RemainingTime -= FPlatformTime::Seconds() - StartIterationTime;
		if (bSlicePlanning && RemainingTime <= 0.0f)
		{
			// No more time for this frame
			break;
		}
	}
}

void UNHTNPlanner::MakeOnePlanStep(FNHTNPlanRequest& Request) const
{
	UNHTNBlackboardComponent& BBComp = *Request.Params.NHTNComponent->GetHTNBBComp();
	UNHTNBaseTask* Task = Request.TasksToVisit.Pop().Get();
	if (const UNHTNCompoundTask* CompoundTask = Cast<UNHTNCompoundTask>(Task))
	{
		const TArray<FNHTNMethod>& Methods = CompoundTask->GetMethods();
		for (const FNHTNMethod& Method : Methods)
		{
			if (NHTNPlannerHelper::CanAllBeExecuted(Method.Decorators, BBComp))
			{
				// Save world state
				Request.SavedWorldStates.Emplace(BBComp.RetrieveBBMemory(), Request.PlanResult.Plan, Request.TasksToVisit);
				NHTNPlannerHelper::PushAllReversed(Request.TasksToVisit, Method.Tasks);
			}
		}
	}
	else
	{
		UNHTNPrimitiveTask* PrimitiveTask = CastChecked<UNHTNPrimitiveTask>(Task);
		if (PrimitiveTask->CanBeExecuted(BBComp))
		{
			PrimitiveTask->ApplyExpectedEffects(BBComp);
			Request.PlanResult.Plan.Add(PrimitiveTask);
		}
		else
		{
			RollbackWorldState(Request, BBComp);
		}
	}
}


void UNHTNPlanner::RollbackWorldState(FNHTNPlanRequest& Request, UNHTNBlackboardComponent& BBComp) const
{
	if (Request.SavedWorldStates.IsEmpty())
	{
		// No possible rollback
		return;
	}
	FNHTNSavedWorldState& LastSavedWorldState = Request.SavedWorldStates.Last();
	Request.PlanResult.Plan = MoveTemp(LastSavedWorldState.Plan);
	Request.TasksToVisit = MoveTemp(LastSavedWorldState.TasksToVisit);
	BBComp.SetBBMemory(LastSavedWorldState.Memory);
	Request.SavedWorldStates.Pop();
}

