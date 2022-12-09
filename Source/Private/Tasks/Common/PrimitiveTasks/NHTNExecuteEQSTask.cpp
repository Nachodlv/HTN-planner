// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Common/PrimitiveTasks/NHTNExecuteEQSTask.h"

// UE Includes
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "EnvironmentQuery/EnvQueryManager.h"

// NHTN Includes
#include "Components/NHTNComponent.h"


UNHTNExecuteEQSTask::UNHTNExecuteEQSTask()
{
	StoreKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UNHTNExecuteEQSTask, StoreKey));
	StoreKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UNHTNExecuteEQSTask, StoreKey), AActor::StaticClass());
}

ENHTNTaskStatus UNHTNExecuteEQSTask::ExecuteTask(UNHTNComponent& HTNComp)
{
	FQueryFinishedSignature Delegate = FQueryFinishedSignature::CreateUObject(this, &UNHTNExecuteEQSTask::OnEQSFinished, &HTNComp);
	QueryID = EQS.Execute(*HTNComp.GetAIOwner(), HTNComp.GetBlackboardComponent(), Delegate);
	return QueryID == INDEX_NONE ? ENHTNTaskStatus::Failed : ENHTNTaskStatus::InProgress;
}

void UNHTNExecuteEQSTask::ApplyExpectedEffects(UNHTNBlackboardComponent& WorldState) const
{
	WorldState.AddExpectedKeyToBeSet(StoreKey.SelectedKeyName);
}

void UNHTNExecuteEQSTask::OnEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> EnvQueryResult,
	UNHTNComponent* HTNComp)
{
	UBlackboardComponent* BBComp = HTNComp->GetBlackboardComponent();
	QueryID = INDEX_NONE;
	if (EnvQueryResult->IsSuccessful())
	{
		StoreKey.ResolveSelectedKey(*BBComp->GetBlackboardAsset());
		bool bStored = true;
		if (BBComp && BBComp->GetKeyType(BBComp->GetKeyID(StoreKey.SelectedKeyName)) == UBlackboardKeyType_Bool::StaticClass())
		{
			BBComp->SetValueAsBool(StoreKey.SelectedKeyName, true);
		}
		else
		{
			const UEnvQueryItemType* ItemTypeCDO = EnvQueryResult->ItemType->GetDefaultObject<UEnvQueryItemType>();
			bStored = ItemTypeCDO->StoreInBlackboard(StoreKey, BBComp,
				EnvQueryResult->RawData.GetData() + EnvQueryResult->Items[0].DataOffset);
		}
		if (ensureMsgf(bStored, TEXT("Key %s could not be stored"), *StoreKey.SelectedKeyName.ToString()))
		{
			FinishLatentTask(*HTNComp, ENHTNTaskStatus::Success);
			return;
		}
	}
	else if (bClearKeyOnFail)
	{
		BBComp->ClearValue(StoreKey.SelectedKeyName);
	}
	FinishLatentTask(*HTNComp, ENHTNTaskStatus::Failed);
}

void UNHTNExecuteEQSTask::AbortTask(UNHTNComponent& HTNComp)
{
	if (QueryID != INDEX_NONE)
	{
		UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(HTNComp.GetWorld());
		QueryManager->AbortQuery(QueryID);
	}
}
