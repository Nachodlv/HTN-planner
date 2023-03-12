#pragma once

// UE Includes
#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "NHTNObjectListerContainer.generated.h"

USTRUCT(BlueprintType)
struct FNHTNObjectListed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (GetOptions = "RetrieveObjectNames"))
	FString Selection;

	UPROPERTY()
	TObjectPtr<UObject> ObjectSelected = nullptr;

	void Reset();
};

USTRUCT(BlueprintType)
struct FNHTNObjectList
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<FNHTNObjectListed> List;
};

UINTERFACE()
class UNHTNObjectListerContainer : public UInterface
{
	GENERATED_BODY()
};

/** Gives the possibility to contain a list with an object select */
class NHTN_API INHTNObjectListerContainer
{
	GENERATED_BODY()

public:
	/** Should be called whenever the objects that are being listed change */
	void OnObjectListChanged();

#if WITH_EDITOR
	/** Binds the selection to the object */
	void OnPostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent);
#endif  // WITH_EDITOR

protected:
	/** Returns the FNHTNObjectList UPROPERTY */
	virtual FNHTNObjectList& GetObjectList() = 0;

	/** Returns the object array that will be listed */
	virtual TArray<UObject*> GetObjectsToList() const = 0;

	/** Returns the listed object display name and its unique index */
	virtual TTuple<int32, FString> RetrieveNameFromListedObject(int32 ListIndex, const UObject* ListedObject) const;

	/** Returns how the object will be displayed in the editor */
	FString RetrieveNameFromListedObject_Internal(int32 ListIndex, const UObject* ListedObject) const;

	/** Used for the meta property "GetOptions" to create the select in the editor */
	UFUNCTION()
	virtual TArray<FString> RetrieveObjectNames() const;

	/** Returns the object that the ObjectListed is currently selecting */
	UObject* RetrieveObjectSelected(const FNHTNObjectListed& ObjectListed, const TArray<UObject*>& Listing);
};
