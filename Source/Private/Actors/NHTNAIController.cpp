#include "Actors/NHTNAIController.h"

// NHTN Includes
#include "Components/NHTNBlackboardComponent.h"
#include "Components/NHTNComponent.h"
#include "Domain/NHTNDomain.h"


void ANHTNAIController::RunHTN()
{
	if (!HTNComponent)
	{
		HTNComponent = NewObject<UNHTNComponent>(this, TEXT("HTNComponent"));
		HTNComponent->RegisterComponent();
	}
	BrainComponent = HTNComponent;

	if (!HTNBBComp)
	{
		HTNBBComp= NewObject<UNHTNBlackboardComponent>(this, FName(TEXT("NHTNBlackboardComponent")));
		HTNBBComp->RegisterComponent();
	}
	
	if (const UNHTNDomain* DomainLoaded = Domain.LoadSynchronous())
	{
		UBlackboardComponent* BBComp;
		UseBlackboard(DomainLoaded->GetBlackboardAsset(), BBComp);
	}
	HTNComponent->SetDomain(Domain);
	HTNComponent->StartLogic();
}


