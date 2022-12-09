// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug/NHTNGameplayDebugger_HTN.h"

// UE Includes
#include "AIController.h"

// NHTHN Includes
#include "Components/NHTNComponent.h"
#include "Tasks/NHTNCompoundTask.h"
#include "Tasks/NHTNDecorator.h"

namespace NHTNGameplayDebugger
{
	FString DescribeColorHelper(const FColor& Color)
	{
		const int32 MaxColors = GColorList.GetColorsNum();
		for (int32 Idx = 0; Idx < MaxColors; ++Idx)
		{
			if (Color == GColorList.GetFColorByIndex(Idx))
			{
				return GColorList.GetColorNameByIndex(Idx);
			}
		}
		return FString(TEXT("color"));
	}

	FString SetTextInColor(const FString& Text, const FColor& Color)
	{
		 return FString::Printf(TEXT("{%s}'%s'"), *DescribeColorHelper(Color), *Text);
	}

	FString GetNodeDescription(const UNHTNBaseTask* Task, bool bRunningTask, const UNHTNBlackboardComponent& BBComp)
	{
		FString Description = Task->GetTitleDescription();
		if (bRunningTask)
		{
			Description += FString::Printf(TEXT(": %s"), *Task->GetRuntimeDescription(BBComp));
		}
		return SetTextInColor(MoveTemp(Description), bRunningTask ? FColor::Blue : FColor::Red);
	}
}

TSharedRef<FGameplayDebuggerCategory> FNHTNGameplayDebugger_HTN::MakeInstance()
{
	return MakeShareable(new FNHTNGameplayDebugger_HTN());
}

FNHTNGameplayDebugger_HTN::FNHTNGameplayDebugger_HTN()
{
	SetDataPackReplication<FNHTNDebugData>(&DebugData);
}

void FNHTNGameplayDebugger_HTN::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	const UNHTNComponent* HTNComp = nullptr;
	if (const APawn* Pawn = Cast<APawn>(DebugActor))
	{
		if (const AAIController* Controller = Cast<AAIController>(Pawn->GetController()))
		{
			HTNComp = Cast<UNHTNComponent>(Controller->GetBrainComponent());
		}
	}
	if (!HTNComp)
	{
		return;
	}
	
	const UNHTNBlackboardComponent& BBComp = *HTNComp->GetHTNBBComp();

	DebugData.DebugPlan.Reset();

	const UNHTNBaseTask* LastParent = nullptr;
	FNHTNDebugNode* LastNode = nullptr;
	for (int32 i = 0; i < HTNComp->Plan.Num(); ++i)
	{
		const bool bRunningTask = HTNComp->CurrentTask == i;
		
		const UNHTNPrimitiveTask* PlanTask = HTNComp->Plan[i];
		FNHTNDebugNode DebugNode;

		const UNHTNBaseTask* CurrentTask = PlanTask;
		const UNHTNBaseTask* ParentTask = CurrentTask->GetParentTask();
		FString Description = CurrentTask->GetTitleDescription();
		
		DebugNode.DebugText = NHTNGameplayDebugger::GetNodeDescription(CurrentTask, bRunningTask, BBComp);

		while (ParentTask && ParentTask != LastParent)
		{
			CurrentTask = ParentTask;
			ParentTask = CurrentTask->GetParentTask();
			
			FNHTNDebugNode NewParent;
			NewParent.Children.Add(MoveTemp(DebugNode));
			NewParent.DebugText = NHTNGameplayDebugger::GetNodeDescription(CurrentTask, bRunningTask, BBComp);
			DebugNode = MoveTemp(NewParent);
		}
		if (ParentTask)
		{
			// We matched with our previous parent
			LastNode->Children.Add(MoveTemp(DebugNode));
		}
		else
		{
			LastParent = CurrentTask;
			LastNode = &DebugData.DebugPlan.Add_GetRef(MoveTemp(DebugNode));
		}
	}

	for (FNHTNDebugNode& DebugNode : DebugData.DebugPlan)
	{
		AddTextLine(DebugNode.ToString(0));
	}
}

void FNHTNGameplayDebugger_HTN::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
}

FArchive& FNHTNGameplayDebugger_HTN::FNHTNDebugNode::operator<<(FArchive& Ar)
{
	Serialize(Ar);
	return Ar;
}

void FNHTNGameplayDebugger_HTN::FNHTNDebugNode::Serialize(FArchive& Ar)
{
	Ar << DebugText;
	Ar << Children;
}

FString FNHTNGameplayDebugger_HTN::FNHTNDebugNode::ToString(int32 Depth) const
{
	FString ChildrenString = DebugText;
	++Depth;
	for (const FNHTNDebugNode& Node : Children)
	{
		ChildrenString += TEXT("\n");
		for (int32 i = 0; i < Depth; ++i)
		{
			ChildrenString += TEXT("\t");
		}
		ChildrenString += FString::Printf(TEXT("%s"), *Node.ToString(Depth));
	}
	return ChildrenString;
}

void FNHTNGameplayDebugger_HTN::FNHTNDebugData::Serialize(FArchive& Ar)
{
	Ar << DebugPlan;
}

