#pragma once

// UE Includes
#include "CoreMinimal.h"
#include "UObject/Object.h"

// NHTN Includes
#include "Components/NHTNBlackboardComponent.h"

#include "NHTNPlanner.generated.h"

enum class ENHTNPlanState : uint8
{
	/** The plan not started to be generated */
	Waiting,
	/** The plan is currently being generated */
	InProgress,
	/** The planner failed to generate a plan */
	Failed,
	/** The planner generated the plan successfully */
	Finished,
};

typedef TArray<TWeakObjectPtr<UNHTNBaseTask>> FWeakTasks;
typedef TArray<TWeakObjectPtr<UNHTNPrimitiveTask>> FWeakPrimitiveTasks;

struct FNHTNPlanResult
{
	/** The actual plan */
	FWeakPrimitiveTasks Plan;
	
	/** The current state of the plan */
	ENHTNPlanState State = ENHTNPlanState::Waiting;
};

struct FNHTNSavedWorldState
{
	FNHTNSavedWorldState(FNHTNBlackboardMemory&& InMemory, const FWeakPrimitiveTasks& InPlan,
		const FWeakTasks& InTasksToVisit)
		: Memory(MoveTemp(InMemory)), Plan(InPlan), TasksToVisit(InTasksToVisit) {}
	
	FNHTNBlackboardMemory Memory;
	FWeakPrimitiveTasks Plan;
	FWeakTasks TasksToVisit;
};

DECLARE_DELEGATE_OneParam(FNHTNOnPlanFinished, FNHTNPlanResult);

/** Parameters used to request the generation of a plan */
struct FNHTNPlanRequestParams
{
	/** The component making the request */
	TWeakObjectPtr<UNHTNComponent> NHTNComponent = nullptr;

	/** The domain tasks from where the plan will be generated */
	TArray<TWeakObjectPtr<UNHTNBaseTask>> TasksInstances;

	/** Broadcasts when the plan is generated or fails */
	FNHTNOnPlanFinished Delegate;
};

/** Contains the information of a plan request */
struct FNHTNPlanRequest
{
	FNHTNPlanRequest(FNHTNPlanRequestParams&& InParams) : Params(MoveTemp(InParams)) {}

	/** The parameters for the request */
	FNHTNPlanRequestParams Params;

	/** Unique reference for the request */
	int32 UniqueID = INDEX_NONE;

	/** World states used to rollback when planning */
	TArray<FNHTNSavedWorldState> SavedWorldStates;

	/** Tasks that needs to be processed */
	FWeakTasks TasksToVisit;

	/** The plan itself */
	FNHTNPlanResult PlanResult;

	/** Whether the plan request started been generated */
	bool IsWaitingToBeProcessed() const { return PlanResult.State == ENHTNPlanState::Waiting; }
};

UCLASS(Config = "NHTN")
class NHTN_API UNHTNPlanner : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UNHTNPlanner();

	/** Request the generation of a new plan. Returns the request ID */
	int32 GeneratePlan(FNHTNPlanRequestParams&& Request);

	/** Stops the generation of the plan with the given ID */
	void AbortPlan(int32 RequestID);

	// ~ Begin FTickableGameObject
	virtual TStatId GetStatId() const override;
	
	virtual bool IsTickable() const override;

	/** Process the plan requests */
	virtual void Tick(float DeltaTime) override;
	// ~ End FTickableGameObject
	
	UPROPERTY(Config)
	FSoftClassPath PlannerClass;

protected:
	/** Make one iteration the in the generation of the plan request */
	void MakeOnePlanStep(FNHTNPlanRequest& Request) const;

	/** Rollbacks the world state request with the last one saved */
	void RollbackWorldState(FNHTNPlanRequest& Request, UNHTNBlackboardComponent& BBComp) const;
	
private:
	/** The current waiting plan generation requests */
	TArray<FNHTNPlanRequest> Requests;

	/** How much time the planner will take every frame at maximum to generate the plans */
	UPROPERTY(Config)
	float MaxTimePlanning = 0.01f;

	/** Whether we want to slice the generation of plans in each frame */
	UPROPERTY(Config)
	bool bSlicePlanning = true;

	/** Whether we want to calculate all the plans at the same time.
	 * If false, the planner will finish one plan and then starts with the next one. */
	UPROPERTY(Config)
	bool bPlanUsingBreadth = true;

	/** Which request should be calculated next  */
	int32 NextRequestIndex = 0;
};
