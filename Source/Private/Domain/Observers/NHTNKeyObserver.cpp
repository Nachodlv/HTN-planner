#include "Domain/Observers/NHTNKeyObserver.h"

// UE Includes
#include "VisualLogger/VisualLogger.h"

// NHTN Includes
#include "Components/NHTNComponent.h"
#include "Domain/NHTNDomain.h"
#include "Tasks/NHTNPrimitiveTask.h"
#include "Types/NHTNTypes.h"

void UNHTNKeyObserver::Initialize(UNHTNComponent* InHTNComp)
{
	HTNComp = InHTNComp;
	HTNComp->OnCurrenTaskStateChanged.AddDynamic(this, &UNHTNKeyObserver::OnCurrentTaskStateChanged);
}

EBlackboardNotificationResult UNHTNKeyObserver::OnKeyObservedChange(const UBlackboardComponent& BlackboardComponent,
	FBlackboard::FKey KeyID)
{
	ResetCurrentPlan();
	return EBlackboardNotificationResult::ContinueObserving;
}

void UNHTNKeyObserver::OnCurrentTaskStateChanged(UNHTNComponent* HtnComponent, UNHTNPrimitiveTask* Task, ENHTNTaskStatus NewStatus)
{
	bool bNewRelevancy = false;

	// TODO (Ignacio) revisit once multiple HTN plan can be ran at the same time
	if (NewStatus == ENHTNTaskStatus::InProgress)
	{
		if (RelevantTasks.List.IsEmpty())
		{
			bNewRelevancy = true;
		}
		else
		{
			// If a task is running check whether is contained in the RelevantTasks
			const int32 TaskIndex = Task->GetDomainIndex();
			for (const FNHTNObjectListed& ObjectList : RelevantTasks.List)
			{
				if (Cast<UNHTNPrimitiveTask>(ObjectList.ObjectSelected)->GetDomainIndex() == TaskIndex)
				{
					bNewRelevancy = true;
					break;
				}
			}
		}
	}
	
	if (bNewRelevancy == bRelevant)
	{
		return;
	}
	bRelevant = bNewRelevancy;
	if (bRelevant)
	{
		OnBeginRelevance();
	}
	else
	{
		OnCeaseRelevance();
	}
}

void UNHTNKeyObserver::OnBeginRelevance()
{
	UBlackboardComponent* BBComp = HTNComp->GetBlackboardComponent();
	BlackboardKey.ResolveSelectedKey(*BBComp->GetBlackboardAsset());
	const FOnBlackboardChangeNotification Delegate = FOnBlackboardChangeNotification::CreateUObject(this,
		&UNHTNKeyObserver::OnKeyObservedChange);
	BBComp->RegisterObserver(BlackboardKey.GetSelectedKeyID(), this, Delegate);
}

void UNHTNKeyObserver::OnCeaseRelevance()
{
	UBlackboardComponent* BBComp = HTNComp ? HTNComp->GetBlackboardComponent() : nullptr;
	if (BBComp)
	{
		BBComp->UnregisterObserversFrom(this);
	}
}

void UNHTNKeyObserver::ResetCurrentPlan() const
{
	if (!HTNComp->IsRunning())
	{
		return;
	}
	
	UE_VLOG(HTNComp->GetOwner(), LogNHTN, Log, TEXT("%s notifying key %s changed (%s)"), *GetName(),
		*BlackboardKey.SelectedKeyName.ToString(), *NHTN_ENUM_TO_STRING(ENHTNObservedKeyReplan, ReplanType));

	if (ReplanType == ENHTNObservedKeyReplan::AbortTask)
	{
		HTNComp->RestartLogic();
	}
	else
	{
		HTNComp->ResetPlan();
	}
}

TArray<UObject*> UNHTNKeyObserver::GetObjectsToList() const
{
	const FNHTNPrimitiveTaskIterator It (Domain->GetTasks());
	TArray<UObject*> Tasks;
	FNHTNArrayUtils::FromOneTypeArrayToAnother(It.GetTasksToVisit(), Tasks);
	return Tasks;
}

TTuple<int32, FString> UNHTNKeyObserver::RetrieveNameFromListedObject(int32 ListIndex,
	const UObject* ListedObject) const
{
	const UNHTNPrimitiveTask* Task = Cast<UNHTNPrimitiveTask>(ListedObject);
	return MakeTuple(Task->GetDomainIndex(), Task->GetTitleDescription());
}

void UNHTNKeyObserver::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	OnPostEditChangeChainProperty(PropertyChangedEvent);
}

void UNHTNKeyObserver::BeginDestroy()
{
	if (HTNComp)
	{
		HTNComp->OnCurrenTaskStateChanged.RemoveDynamic(this, &UNHTNKeyObserver::OnCurrentTaskStateChanged);
	}
	if (bRelevant)
	{
		OnCeaseRelevance();
		bRelevant = false;
	}
	Super::BeginDestroy();
}
