#include "INHTNModule.h"

#define LOCTEXT_NAMESPACE "FNHTNModule"

class FNHTNModule : public INHTNModule
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override
	{
		// This is loaded upon first request
	}

	void ShutdownModule() override
	{
	}
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNHTNModule, NHTN);
