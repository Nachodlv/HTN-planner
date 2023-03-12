#pragma once

// UE Includes
#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardAssetProvider.h"
#include "BehaviorTree/BlackboardData.h"

#include "NHTNDomain.generated.h"

class UNHTNPrimitiveTask;
class UNHTNKeyObserver;
class UNHTNBaseTask;

/** Iterates the primitive tasks from the given base tasks taking into account the possible compound tasks */
struct FNHTNPrimitiveTaskIterator
{
public:
	FNHTNPrimitiveTaskIterator(const TArray<TObjectPtr<UNHTNBaseTask>>& InTasks);
	FNHTNPrimitiveTaskIterator(const TArray<UNHTNBaseTask*>& InTasks);

	UNHTNPrimitiveTask* Get() const { return TasksToVisit[Index].Get(); }
	UNHTNPrimitiveTask* operator->() const { return Get(); }
	UNHTNPrimitiveTask* operator*() const { return Get(); }

	void operator++() { ++Index; }

	operator bool() const { return TasksToVisit.IsValidIndex(Index); }

	UNHTNPrimitiveTask* operator[](const int32 InIndex) { return TasksToVisit[InIndex].Get(); }

	bool IsValidIndex(int32 InIndex) const { return TasksToVisit.IsValidIndex(InIndex); }

	const TArray<TWeakObjectPtr<UNHTNPrimitiveTask>>& GetTasksToVisit() const { return TasksToVisit; }
	
private:
	void InitializeTasksToVisit(const TArray<UNHTNBaseTask*>& InTasks);

	TArray<TWeakObjectPtr<UNHTNPrimitiveTask>> TasksToVisit;
	
	int32 Index = 0;
};

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
