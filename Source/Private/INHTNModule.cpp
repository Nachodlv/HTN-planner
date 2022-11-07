#include "INHTNModule.h"

#include "GameplayDebugger.h"

#include "Debug/NHTNGameplayDebugger_HTN.h"

#define LOCTEXT_NAMESPACE "FNHTNModule"

class FNHTNModule : public INHTNModule
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override
	{
#if WITH_GAMEPLAY_DEBUGGER
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.RegisterCategory(FNHTNGameplayDebugger_HTN::GetDebuggerConfidenceCategoryName(),
			IGameplayDebugger::FOnGetCategory::CreateStatic(&FNHTNGameplayDebugger_HTN::MakeInstance));
#endif  // WITH_GAMEPLAY_DEBUGGER
	}

	void ShutdownModule() override
	{
#if WITH_GAMEPLAY_DEBUGGER
		if (IGameplayDebugger::IsAvailable())
		{
			IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
			GameplayDebuggerModule.UnregisterCategory(FNHTNGameplayDebugger_HTN::GetDebuggerConfidenceCategoryName());
		}
#endif // WITH_GAMEPLAY_DEBUGGER
	}
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNHTNModule, NHTN);
