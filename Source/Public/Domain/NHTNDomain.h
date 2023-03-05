#pragma once

// UE Includes
#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardAssetProvider.h"
#include "BehaviorTree/BlackboardData.h"

#include "NHTNDomain.generated.h"

class UNHTNKeyObserver;
class UNHTNBaseTask;

UCLASS()
class NHTN_API UNHTNDomain : public UDataAsset, public IBlackboardAssetProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	const TArray<UNHTNBaseTask*>& GetTasks() const { return Tasks; }

	/** Returns the objects used to observe key changes and notify the HTN component about it */
	const TArray<TObjectPtr<UNHTNKeyObserver>>& GetObservedKeys() const { return ObservedKeys; }

	// ~ Begin IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override { return BlackboardData.LoadSynchronous(); };
	// ~ End IBlackboardAssetProvider

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void ResolveBlackboardKeysFromObject(UObject* Object) const;
	void ResolveBlackboardKeySelector(FBlackboardKeySelector& KeySelector) const;
#endif // WITH_EDITOR
	
private:
	UPROPERTY(EditAnywhere, Instanced, Category = "NHTN")
	TArray<TObjectPtr<UNHTNBaseTask>> Tasks;

	UPROPERTY(EditAnywhere, Category = "NHTN")
	TSoftObjectPtr<UBlackboardData> BlackboardData;

	/** Used to observe key changes and notify the HTN component about it */
	UPROPERTY(EditAnywhere, Category = "NHTN", Instanced)
	TArray<TObjectPtr<UNHTNKeyObserver>> ObservedKeys;
};
