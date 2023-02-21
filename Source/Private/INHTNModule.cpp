#include "INHTNModule.h"

// UE Includes
#include "GameplayDebugger.h"

// NHTN Includes
#include "Debug/NHTNGameplayDebugger_HTN.h"
#include "Planner/NHTNPlanner.h"

#define LOCTEXT_NAMESPACE "FNHTNModule"

class FNHTNModule : public INHTNModule
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override
	{
		INHTNModule::StartupModule();
#if WITH_GAMEPLAY_DEBUGGER
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.RegisterCategory(FNHTNGameplayDebugger_HTN::GetDebuggerConfidenceCategoryName(),
			IGameplayDebugger::FOnGetCategory::CreateStatic(&FNHTNGameplayDebugger_HTN::MakeInstance));
#endif  // WITH_GAMEPLAY_DEBUGGER
	}

	void ShutdownModule() override
	{
		INHTNModule::ShutdownModule();
#if WITH_GAMEPLAY_DEBUGGER
		if (IGameplayDebugger::IsAvailable())
		{
			IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
			GameplayDebuggerModule.UnregisterCategory(FNHTNGameplayDebugger_HTN::GetDebuggerConfidenceCategoryName());
		}
#endif // WITH_GAMEPLAY_DEBUGGER
	}
};

void INHTNModule::StartupModule()
{
	Planner = nullptr;
}

void INHTNModule::ShutdownModule()
{
	if (IsValid(Planner) && Planner->IsRooted())
    {
    	Planner->RemoveFromRoot();
    }
    Planner = nullptr;
}

UNHTNPlanner* INHTNModule::GetPlanner()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_INHTNModule_GetPlanner);
	if (!Planner)
	{
		const FSoftClassPath PlannerClassName =
			(UNHTNPlanner::StaticClass()->GetDefaultObject<UNHTNPlanner>())->PlannerClass;

		const UClass* SingletonClass = PlannerClassName.TryLoadClass<UObject>();
		checkf(SingletonClass != nullptr, TEXT("Common config value PlannerClass is not a valid class name."));

		Planner = NewObject<UNHTNPlanner>(GetTransientPackage(), SingletonClass, NAME_None);
		Planner->AddToRoot();
	}
	check(Planner);
	return Planner;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNHTNModule, NHTN);


