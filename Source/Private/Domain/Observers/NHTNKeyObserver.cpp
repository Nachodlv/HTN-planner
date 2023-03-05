#include "Domain/Observers/NHTNKeyObserver.h"

// UE Includes
#include "VisualLogger/VisualLogger.h"

// NHTN Includes
#include "Components/NHTNComponent.h"
#include "Types/NHTNTypes.h"


void UNHTNKeyObserver::Initialize(UNHTNComponent* InHTNComp)
{
	HTNComp = InHTNComp;
	UBlackboardComponent* BBComp = HTNComp->GetBlackboardComponent();
	BlackboardKey.ResolveSelectedKey(*BBComp->GetBlackboardAsset());
	const FOnBlackboardChangeNotification Delegate = FOnBlackboardChangeNotification::CreateUObject(this,
		&UNHTNKeyObserver::OnKeyObservedChange);
	BBComp->RegisterObserver(BlackboardKey.GetSelectedKeyID(), this, Delegate);
}

EBlackboardNotificationResult UNHTNKeyObserver::OnKeyObservedChange(const UBlackboardComponent& BlackboardComponent,
	uint8 KeyID)
{
	NotifyKeyChange();
	return EBlackboardNotificationResult::ContinueObserving;
}

void UNHTNKeyObserver::NotifyKeyChange() const
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

void UNHTNKeyObserver::BeginDestroy()
{
	UBlackboardComponent* BBComp = HTNComp ? HTNComp->GetBlackboardComponent() : nullptr;
	if (BBComp)
	{
		BBComp->UnregisterObserversFrom(this);
	}
	Super::BeginDestroy();
}
