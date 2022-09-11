#include "Tasks/NHTNPrimitiveTask.h"

// NHTN Includes
#include "Components/NHTNComponent.h"

void UNHTNPrimitiveTask::FinishLatentTask(UNHTNComponent& HTNComp, ENHTNTaskStatus Status)
{
	HTNComp.FinishLatentTask(Status);
}
