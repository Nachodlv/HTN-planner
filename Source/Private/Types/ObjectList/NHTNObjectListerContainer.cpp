#include "Types/ObjectList/NHTNObjectListerContainer.h"

#if WITH_EDITOR
#include "Misc/MessageDialog.h"
#endif // WITH_EDITOR

#define LOCTEXT_NAMESPACE "INHTNObjectListerContainer" 

void FNHTNObjectListed::Reset()
{
	Selection.Reset();
	ObjectSelected = nullptr;
}

FString INHTNObjectListerContainer::RetrieveNameFromListedObject_Internal(int32 Index, const UObject* Object) const
{
	TTuple<int32, FString> NameFromListedObject = RetrieveNameFromListedObject(Index, Object);
	return FString::Printf(TEXT("(%d) %s"), NameFromListedObject.Get<0>(), *NameFromListedObject.Get<1>());
}

TArray<FString> INHTNObjectListerContainer::RetrieveObjectNames() const
{
	TArray<FString> Result;
	TArray<UObject*> Listing = GetObjectsToList();
	Result.Reserve(Listing.Num());
	for (int32 i = 0; i < Listing.Num(); ++i)
	{
		Result.Add(RetrieveNameFromListedObject_Internal(i, Listing[i]));
	}
	return Result;
}

void INHTNObjectListerContainer::OnObjectListChanged()
{
	const TArray<UObject*> Listing = GetObjectsToList();
	FNHTNObjectList& ObjectList = GetObjectList();

#if WITH_EDITOR
	TArray<FString> ObjetsListedMissing;
#endif // WITH_EDITOR

	for (FNHTNObjectListed& ObjectListed : ObjectList.List)
	{
		const int32 Index = Listing.IndexOfByKey(ObjectListed.ObjectSelected);
		if (Listing.IsValidIndex(Index))
		{
			ObjectListed.Selection = RetrieveNameFromListedObject_Internal(Index, ObjectListed.ObjectSelected.Get());
		}
		else
		{
#if WITH_EDITOR
			ObjetsListedMissing.Add(MoveTemp(ObjectListed.Selection));
#endif // WITH_EDITOR
			ObjectListed.Reset();
		}
	}

#if WITH_EDITOR
	if (ObjetsListedMissing.IsEmpty())
	{
		return;
	}
	FString ObjectsMissing;
	for (const FString& MissingObject : ObjetsListedMissing)
	{
		ObjectsMissing += FString::Printf(TEXT("\n%s"), *MissingObject);
	}
	const FText Message = FText::Format(LOCTEXT("ObjectsListMissing", "The following object from the list are missing {0}"),
		FText::FromString(ObjectsMissing));
	FMessageDialog::Open(EAppMsgType::Ok, Message);
#endif // WITH_EDITOR
	
}

TTuple<int32, FString> INHTNObjectListerContainer::RetrieveNameFromListedObject(int32 ListIndex,
	const UObject* ListedObject) const
{
	return MakeTuple(ListIndex, GetNameSafe(ListedObject));
}

void INHTNObjectListerContainer::OnPostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FNHTNObjectListed, Selection))
	{
		const FString RelevantTasksName = GET_MEMBER_NAME_CHECKED(FNHTNObjectList, List).ToString();
		const int32 Index = PropertyChangedEvent.GetArrayIndex(RelevantTasksName);
		TArray<FNHTNObjectListed>& ObjectsListed = GetObjectList().List;
		if (!ObjectsListed.IsValidIndex(Index))
		{
			return;
		}

		FNHTNObjectListed& ObjectListed = ObjectsListed[Index];
		ObjectListed.ObjectSelected = RetrieveObjectSelected(ObjectListed, GetObjectsToList());
	}
}

UObject* INHTNObjectListerContainer::RetrieveObjectSelected(const FNHTNObjectListed& ObjectListed,
	const TArray<UObject*>& Listing)
{
	if (ObjectListed.Selection.Len() < 2)
	{
		return nullptr;
	}
	FString Right;
	ObjectListed.Selection.Split(TEXT("("), nullptr, &Right);
	FString IndexString;
	Right.Split(TEXT(")"), &IndexString, nullptr);
	const int32 TaskIndex = FCString::Atoi(*IndexString);
	return Listing.IsValidIndex(TaskIndex) ? Listing[TaskIndex] : nullptr;
}

#undef LOCTEXT_NAMESPACE
