// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Common/Decorators/NHTNIsBBKeySetDecorator.h"

bool UNHTNIsBBKeySetDecorator::CalculateRawCondition(const UNHTNBlackboardComponent& WorldState) const
{
	const FBlackboard::FKey KeyID = WorldState.GetKeyID(BBKey.SelectedKeyName);
	if (WorldState.IsKeyExpectedToBeSet(KeyID))
	{
		return true;
	}
	const TSubclassOf<UBlackboardKeyType> KeyType = WorldState.GetKeyType(KeyID);
	const UBlackboardKeyType* KeyCDO = KeyType->GetDefaultObject<UBlackboardKeyType>();
	const uint8* KeyMemory = WorldState.GetKeyRawData(KeyID);
	return KeyCDO->WrappedTestBasicOperation(WorldState, KeyMemory, EBasicKeyOperation::Set);
}
