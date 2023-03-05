#include "Tasks/NHTNPrimitiveTask.h"

// NHTN Includes
#include "Components/NHTNComponent.h"

void UNHTNPrimitiveTask::OnWrappedMessage(UBrainComponent* BrainComponent, const FAIMessage& Message)
{
	OnMessage(*Cast<UNHTNComponent>(BrainComponent), Message);
}

void UNHTNPrimitiveTask::FinishLatentTask(UNHTNComponent& HTNComp, ENHTNTaskStatus Status)
{
	HTNComp.FinishLatentTask(this, Status);
}

void UNHTNPrimitiveTask::RegisterMessageObserver(UNHTNComponent& HTNComp, const FName& Message,
	const FAIRequestID& InRequestID)
{
	HTNComp.RegisterMessageObserver(this, Message, InRequestID);
}

void UNHTNPrimitiveTask::RegisterMessageObserver(UNHTNComponent& HTNComp, const FName& Message)
{
	HTNComp.RegisterMessageObserver(this, Message);
}

void UNHTNPrimitiveTask::UnRegisterMessageObserver(UNHTNComponent& HTNComp, const FName& Message,
	const FAIRequestID& InRequestID) const
{
	HTNComp.UnRegisterMessageObserver(this, Message, InRequestID);
}
