#pragma once

// UE Includes
#include "BehaviorTree/BlackboardComponent.h"
#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "NHTNBaseTask.h"

#include "NHTNPrimitiveTask.generated.h"

class UNHTNComponent;


UCLASS(Abstract)
class NHTN_API UNHTNPrimitiveTask : public UNHTNBaseTask
{
	GENERATED_BODY()

public:
	/** Applies the corresponding effects to the world state when the task is executed successfully */
	virtual void ApplyEffects(UBlackboardComponent& WorldState) const {}

	/** Applies the corresponding effects to the world state as if the task was executed. Only used on planning. */
	virtual void ApplyExpectedEffects(UNHTNBlackboardComponent& WorldState) const {}

	/** Called when the brain component received a message. Calls UNHTNPrimitiveTask::OnMessage */
	void OnWrappedMessage(UBrainComponent* BrainComponent, const FAIMessage& Message);

	/** Receives the messages registered by UNHTNPrimitiveTask::RegisterMessageObserver */
	virtual void OnMessage(UNHTNComponent& HTNComp, const FAIMessage& Message) {}

protected:
	/** Changes the state of the current task */
	void FinishLatentTask(UNHTNComponent& HTNComp, ENHTNTaskStatus Status);

	/** Registers to the given Message */
	void RegisterMessageObserver(UNHTNComponent& HTNComp, const FName& Message);
	void RegisterMessageObserver(UNHTNComponent& HTNComp, const FName& Message, const FAIRequestID& InRequestID);

	/** Unregisters from previous registered messages */
	void UnRegisterMessageObserver(UNHTNComponent& HTNComp, const FName& Message,
		const FAIRequestID& InRequestID = FAIRequestID::AnyRequest) const;
};
