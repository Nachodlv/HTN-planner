#include "Domain/NHTNDomain.h"

// NHTN Includes
#include "Domain/Observers/NHTNKeyObserver.h"
#include "Tasks/NHTNBaseTask.h"
#include "Tasks/NHTNCompoundTask.h"

FNHTNPrimitiveTaskIterator::FNHTNPrimitiveTaskIterator(const TArray<TObjectPtr<UNHTNBaseTask>>& InTasks)
{
	InitializeTasksToVisit(InTasks);
}

FNHTNPrimitiveTaskIterator::FNHTNPrimitiveTaskIterator(const TArray<UNHTNBaseTask*>& InTasks)
{
	InitializeTasksToVisit(InTasks);
}

void FNHTNPrimitiveTaskIterator::InitializeTasksToVisit(const TArray<UNHTNBaseTask*>& InTasks)
{
	TArray<UNHTNBaseTask*> TasksToProcess= InTasks;
	Algo::Reverse(TasksToProcess);
	while (!TasksToProcess.IsEmpty())
	{
		UNHTNBaseTask* Task = TasksToProcess.Pop();
		if (!Task)
		{
			continue;
		}
		if (const UNHTNCompoundTask* CompoundTask = Cast<UNHTNCompoundTask>(Task))
		{
			for (const FNHTNMethod& Method : CompoundTask->GetMethods())
			{
				TArray<UNHTNBaseTask*> MethodTasks = Method.Tasks;
				Algo::Reverse(MethodTasks);
				TasksToProcess.Append(MethodTasks);
			}
		}
		else
		{
			TasksToVisit.Add(CastChecked<UNHTNPrimitiveTask>(Task));
		}
	}
}

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
				ObservedKey->SetUpDomain(this);
			}
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UNHTNDomain, Tasks))
	{
		int32 Index = 0;
		for (FNHTNPrimitiveTaskIterator It (Tasks); It; ++It)
		{
			It->SetDomainIndex(Index);
			++Index;
		}
		for (UNHTNKeyObserver* KeyObserver : ObservedKeys)
		{
			if (KeyObserver)
			{
				KeyObserver->OnObjectListChanged();
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
