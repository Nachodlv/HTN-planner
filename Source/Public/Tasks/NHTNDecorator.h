#pragma once

// UE Includes
#include "CoreMinimal.h"

// NHTN Includes
#include "NHTNBaseNode.h"

#include "NHTNDecorator.generated.h"

// TODO (Ignacio) do we really need this class?
/** A task with a condition. Used for compound tasks */
UCLASS()
class NHTN_API UNHTNDecorator : public UNHTNBaseNode
{
	GENERATED_BODY()
};
