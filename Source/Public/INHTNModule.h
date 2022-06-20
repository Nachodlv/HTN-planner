#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class INHTNModule : public IModuleInterface
{
public:

	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline INHTNModule& Get()
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_INHTNModule_Get);
		static INHTNModule& Singleton = FModuleManager::LoadModuleChecked< INHTNModule >("NHTN");
		return Singleton;
	}

	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_IPMTAIModule_IsAvailable);
		return FModuleManager::Get().IsModuleLoaded("NHTN");
	}
};
