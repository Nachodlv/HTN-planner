#include "Actors/NHTNAIController.h"

// NHTN Includes
#include "Components/NHTNComponent.h"


void ANHTNAIController::RunHTN()
{
	if (!HTNComponent)
	{
		HTNComponent = NewObject<UNHTNComponent>(this, TEXT("HTNComponent"));
		HTNComponent->RegisterComponent();
	}
	BrainComponent = HTNComponent;
	HTNComponent->SetDomain(Domain);
	HTNComponent->StartLogic();
}


