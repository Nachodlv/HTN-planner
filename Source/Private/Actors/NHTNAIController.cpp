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
	Blackboard = HTNBBComp;
	
	if (const UNHTNDomain* DomainLoaded = Domain.LoadSynchronous())
	{
		InitializeBlackboard(*HTNBBComp, *DomainLoaded->GetBlackboardAsset());
	}
	HTNComponent->SetDomain(Domain);
	HTNComponent->StartLogic();
}


