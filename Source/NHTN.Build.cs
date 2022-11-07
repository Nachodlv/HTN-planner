// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NHTN : ModuleRules
{
	public NHTN(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"AIModule",
			"NavigationSystem",
			"Engine",
			"Core",
			"CoreUObject",
			"InputCore",
			"Slate",
			"SlateCore",
			"GameplayDebugger"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"RenderCore",
			"RHI",
		});

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
