#pragma once

#if WITH_GAMEPLAY_DEBUGGER

// UE Includes
#include "CoreMinimal.h"
#include "GameplayDebuggerCategory.h"

/** Debugger for the NHTNComponent */
class NHTN_API FNHTNGameplayDebugger_HTN : public FGameplayDebuggerCategory
{
public:
	static TSharedRef<FGameplayDebuggerCategory> MakeInstance();
	
	static FName GetDebuggerConfidenceCategoryName() { return FName(TEXT("HTN")); }

	FNHTNGameplayDebugger_HTN();
	
	virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	
	virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;

protected:
	struct FNHTNDebugNode
	{
		FNHTNDebugNode() {}
		FString DebugText;
		TArray<FNHTNDebugNode> Children;
		
		FNHTNDebugNode(const FNHTNDebugNode& Other) = delete;
		void operator=(const FNHTNDebugNode& Other) = delete;
		FNHTNDebugNode(FNHTNDebugNode&& Other) noexcept
		{
			Move(Other);
		}
		void operator=(FNHTNDebugNode&& Other) noexcept
		{
			Move(Other);
		}
		void Move(FNHTNDebugNode& Other)
		{
			DebugText = MoveTemp(Other.DebugText);
			Children = MoveTemp(Other.Children);
			Other.Children.Reset();
		}
		FString ToString(int32 Depth) const;
	};
	struct FNHTNDebugData
	{
		FNHTNDebugData() {}

		TArray<FString> DebugPlan;
		FString BlackboardValues;

		FNHTNDebugData(const FNHTNDebugData& Other) = delete;
		void operator=(const FNHTNDebugData& Other) = delete;
		FNHTNDebugData(FNHTNDebugData&& Other) noexcept
		{
			Move(Other);
		}
		void operator=(FNHTNDebugData&& Other) noexcept
		{
			Move(Other);
		}
		
		void Serialize(FArchive& Ar);
		void Move(FNHTNDebugData& Other)
		{
			DebugPlan = MoveTemp(Other.DebugPlan);
			BlackboardValues = MoveTemp(Other.BlackboardValues);
			Other.DebugPlan.Reset();
			Other.BlackboardValues.Reset();
		}
	};

	FNHTNDebugData DebugData;
};

#endif // WITH_GAMEPLAY_DEBUGGER
