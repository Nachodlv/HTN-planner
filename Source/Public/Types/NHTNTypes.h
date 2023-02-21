#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNHTN, Log, All);

#define NHTN_ENUM_TO_STRING(EnumType, Value)\
	FindObject<UEnum>(ANY_PACKAGE, TEXT(#EnumType), true)->GetDisplayNameTextByValue(static_cast<int32>(Value)).ToString()
