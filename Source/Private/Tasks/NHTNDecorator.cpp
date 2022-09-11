// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/NHTNDecorator.h"

bool UNHTNDecorator::CanBeExecuted(const UBlackboardComponent& WorldState) const
{
	const bool bResult = CalculateRawCondition(WorldState);
	return bInverse ? !bResult : bResult;
}
