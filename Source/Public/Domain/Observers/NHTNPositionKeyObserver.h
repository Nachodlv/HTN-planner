#pragma once

// UE Includes
#include "CoreMinimal.h"

// NHTN Includes
#include "NHTNKeyObserver.h"

#include "NHTNPositionKeyObserver.generated.h"

/** Observed the change of a position. Actors and vectors key are supported */
UCLASS()
class NHTN_API UNHTNPositionKeyObserver : public UNHTNKeyObserver, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UNHTNPositionKeyObserver();

	virtual void Initialize(UNHTNComponent* InHTNComp) override;

	/** Updates the last key position */
	virtual void OnBeginRelevance() override;

	/** Resets the obervser state */
	virtual void OnCeaseRelevance() override;

	// ~ Begin FTickableGameObject
	virtual bool IsTickable() const override { return bTrackingActorMovement && IsRelevant(); }
	
	/** Checks whether it should notify the HTN component and updates the last key position */
	virtual void Tick(float DeltaTime) override;
	
	virtual TStatId GetStatId() const override;
	// ~ End FTickableGameObject

protected:
	/** Checks whether it should notify the HTN component and updates the last key position */
	virtual EBlackboardNotificationResult OnKeyObservedChange(const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey KeyID) override;

	/** Updates the LastPosition with the current key position */
	void UpdateLastKeyPosition();

	/** Returns whether the NewPosition is outside the DistanceThreshold range from the LastPosition */
	bool IsOutsideDistanceThreshold(FVector NewPosition);

private:
	/** How much the position needs to change so the observer notifies the HTN component */
	UPROPERTY(EditAnywhere, Category = "NHTN")
	float DistanceThreshold = 50.0f;

	/** The last saved position from the key */
	TOptional<FVector> LastPosition;

	/** Whether the key is an actor so we need to tick and observe its location */
	bool bTrackingActorMovement = false;

	float DistanceThresholdSqr = 0.0f;
};
