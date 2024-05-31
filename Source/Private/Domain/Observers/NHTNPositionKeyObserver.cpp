#include "Domain/Observers/NHTNPositionKeyObserver.h"

// UE Includes
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

// NHTN Includes
#include "Components/NHTNComponent.h"

UNHTNPositionKeyObserver::UNHTNPositionKeyObserver()
{
	const FName BlackboardKeyName = (GET_MEMBER_NAME_CHECKED(UNHTNPositionKeyObserver, BlackboardKey));
	BlackboardKey.AddVectorFilter(this, BlackboardKeyName);
	BlackboardKey.AddObjectFilter(this, BlackboardKeyName, AActor::StaticClass());
}

void UNHTNPositionKeyObserver::Initialize(UNHTNComponent* InHTNComp)
{
	Super::Initialize(InHTNComp);

	DistanceThresholdSqr = DistanceThreshold * DistanceThreshold;
}

void UNHTNPositionKeyObserver::OnBeginRelevance()
{
	Super::OnBeginRelevance();
	UpdateLastKeyPosition();
}

void UNHTNPositionKeyObserver::OnCeaseRelevance()
{
	LastPosition.Reset();
	bTrackingActorMovement = false;
	Super::OnCeaseRelevance();
}

void UNHTNPositionKeyObserver::Tick(float DeltaTime)
{
	const AActor* Actor = Cast<AActor>(GetCurrentValue<UBlackboardKeyType_Object>());
	const FVector ActorLocation = Actor->GetActorLocation();
	if (IsOutsideDistanceThreshold(ActorLocation))
	{
		ResetCurrentPlan();
	}
	UpdateLastKeyPosition();
}

void UNHTNPositionKeyObserver::UpdateLastKeyPosition()
{
	bTrackingActorMovement = GetCurrentValue<UBlackboardKeyType_Object>() != nullptr;
	
	FVector Location;
	if (HTNComp->GetBlackboardComponent()->GetLocationFromEntry(BlackboardKey.SelectedKeyName, Location))
	{
		LastPosition = Location;
	}
}

TStatId UNHTNPositionKeyObserver::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UNHTNPositionKeyObserver, STATGROUP_Tickables);
}

EBlackboardNotificationResult UNHTNPositionKeyObserver::OnKeyObservedChange(
	const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey KeyID)
{
	FVector Location;
	if (HTNComp->GetBlackboardComponent()->GetLocationFromEntry(BlackboardKey.SelectedKeyName, Location))
	{
		if (LastPosition.IsSet() && IsOutsideDistanceThreshold(Location))
		{
			ResetCurrentPlan();
		}
	}

	UpdateLastKeyPosition();

	return EBlackboardNotificationResult::ContinueObserving;
}

bool UNHTNPositionKeyObserver::IsOutsideDistanceThreshold(const FVector NewPosition)
{
	return FVector::DistSquared(*LastPosition, NewPosition) > DistanceThresholdSqr;
}
