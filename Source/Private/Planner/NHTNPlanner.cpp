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

int32 UNHTNPlanner::GeneratePlan(FNHTNPlanRequest&& Request)
{
	static int32 RequestID = 0;
	FNHTNPlanRequest& NewRequest = Requests.Add_GetRef(MoveTemp(Request));
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

	MakePlan(Requests[0]);
	Requests.RemoveAt(0);
}

void UNHTNPlanner::MakePlan(const FNHTNPlanRequest& Request) const
{
	UNHTNBlackboardComponent& BBComp = *Request.NHTNComponent->GetHTNBBComp();
	// Save the current memory from the blackboard to restore after the planning has finished
	FNHTNBlackboardMemory InitialWorldState = BBComp.RetrieveBBMemory();
	
	TArray<FNHTNSavedWorldState> SavedWorldStates;
	FNHTNPlanResult PlanResult;
	FWeakTasks TasksToVisit;
	NHTNPlannerHelper::PushAllReversed(TasksToVisit, Request.TasksInstances);
	while (!TasksToVisit.IsEmpty())
	{
		UNHTNBaseTask* Task = TasksToVisit.Pop().Get();
		if (const UNHTNCompoundTask* CompoundTask = Cast<UNHTNCompoundTask>(Task))
		{
			const TArray<FNHTNMethod>& Methods = CompoundTask->GetMethods();
			for (const FNHTNMethod& Method : Methods)
			{
				if (NHTNPlannerHelper::CanAllBeExecuted(Method.Decorators, BBComp))
				{
					// Save world state
					SavedWorldStates.Emplace(BBComp.RetrieveBBMemory(), PlanResult.Plan, TasksToVisit);
					NHTNPlannerHelper::PushAllReversed(TasksToVisit, Method.Tasks);
				}
			}
		}
		else
		{
			UNHTNPrimitiveTask* PrimitiveTask = CastChecked<UNHTNPrimitiveTask>(Task);
			if (PrimitiveTask->CanBeExecuted(BBComp))
			{
				PrimitiveTask->ApplyExpectedEffects(BBComp);
				PlanResult.Plan.Add(PrimitiveTask);
			}
			else
			{
				RollbackWorldState(BBComp, PlanResult.Plan, TasksToVisit, SavedWorldStates);
			}
		}
	}

	// Restore memory from blackboard
	BBComp.SetBBMemory(InitialWorldState);
	PlanResult.Result = PlanResult.Plan.Num() > 0 ? ENHTNPlanResult::Success : ENHTNPlanResult::Failed;
	Request.Delegate.ExecuteIfBound(MoveTemp(PlanResult));
}


void UNHTNPlanner::RollbackWorldState(UNHTNBlackboardComponent& BBComp, FWeakPrimitiveTasks& InPlan,
	FWeakTasks& InTasksToVisit, TArray<FNHTNSavedWorldState>& SavedWorldStates) const
{
	if (SavedWorldStates.IsEmpty())
	{
		// No possible rollback
		return;
	}
	FNHTNSavedWorldState& LastSavedWorldState = SavedWorldStates.Last();
	InPlan = MoveTemp(LastSavedWorldState.Plan);
	InTasksToVisit = MoveTemp(LastSavedWorldState.TasksToVisit);
	BBComp.SetBBMemory(LastSavedWorldState.Memory);
	SavedWorldStates.Pop();
}

