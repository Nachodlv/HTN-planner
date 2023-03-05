#include "Domain/NHTNDomain.h"

// NHTN Includes
#include "Domain/Observers/NHTNKeyObserver.h"
#include "Tasks/NHTNBaseTask.h"

#if WITH_EDITOR

void UNHTNDomain::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();

	const bool bBlackboardChanged = PropertyName == GET_MEMBER_NAME_CHECKED(UNHTNDomain, BlackboardData);
	if (bBlackboardChanged)
	{
		OnBlackboardOwnerChanged.Broadcast(this, GetBlackboardAsset());
	}

	if (bBlackboardChanged || PropertyName == GET_MEMBER_NAME_CHECKED(UNHTNDomain, Tasks))
	{
		for (UNHTNBaseTask* Task : Tasks)
		{
			if (Task)
			{
				ResolveBlackboardKeysFromObject(Task);
			}
		}
	}

	if (bBlackboardChanged || PropertyName == GET_MEMBER_NAME_CHECKED(UNHTNDomain, ObservedKeys))
	{
		for (UNHTNKeyObserver* ObservedKey : ObservedKeys)
		{
			if (ObservedKey)
			{
				ResolveBlackboardKeysFromObject(ObservedKey);
			}
		}
	}
}

#define GET_STRUCT_NAME_CHECKED(StructName) \
	((void)sizeof(StructName), TEXT(#StructName))

void UNHTNDomain::ResolveBlackboardKeysFromObject(UObject* Object) const
{
	for (const FProperty* TestProperty = Object->GetClass()->PropertyLink; TestProperty; TestProperty = TestProperty->PropertyLinkNext)
	{
		const FStructProperty* TestStruct = CastField<FStructProperty>(TestProperty);
		if (TestStruct == nullptr)
		{
			continue;
		}
				
		const FString TypeDesc = TestStruct->GetCPPType(nullptr, CPPF_None);
		if (TypeDesc.Contains(GET_STRUCT_NAME_CHECKED(FBlackboardKeySelector)))
		{
			FBlackboardKeySelector* PropertyValue = TestStruct->ContainerPtrToValuePtr<FBlackboardKeySelector>(Object);
			ResolveBlackboardKeySelector(*PropertyValue);
		}
	}
}

#undef GET_STRUCT_NAME_CHECKED

void UNHTNDomain::ResolveBlackboardKeySelector(FBlackboardKeySelector& KeySelector) const
{
	if (const UBlackboardData* BBData = GetBlackboardAsset())
	{
		KeySelector.ResolveSelectedKey(*BBData);
	} else
	{
		KeySelector.InvalidateResolvedKey();
	}
}


#endif // WITH_EDITOR
