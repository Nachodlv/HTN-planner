#include "Tasks/Common/PrimitiveTasks/NHTNWaitTask.h"

// NHTN Includes
#include "Components/NHTNComponent.h"

ENHTNTaskStatus UNHTNWaitTask::ExecuteTask(UNHTNComponent& HTNComp)
{
	if (TimeToWait <= 0.0f)
	{
		return ENHTNTaskStatus::Success;
	}
	NHTNComponent = &HTNComp;
	FTimerManager& TimerManager = HTNComp.GetWorld()->GetTimerManager();
	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UNHTNWaitTask::OnWaitTimeEnded);
	const float FinalWaitTime = FMath::RandRange(TimeToWait - TimeDeviation, TimeToWait + TimeDeviation);
	TimerManager.SetTimer(TimerHandle, Delegate, FinalWaitTime, false);

	return ENHTNTaskStatus::InProgress;
}

void UNHTNWaitTask::AbortTask(UNHTNComponent& HTNComp)
{
	FTimerManager& TimerManager = HTNComp.GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(TimerHandle);
}

void UNHTNWaitTask::OnWaitTimeEnded()
{
	FinishLatentTask(*NHTNComponent, ENHTNTaskStatus::Success);
}
