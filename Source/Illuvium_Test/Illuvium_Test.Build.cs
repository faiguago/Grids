// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Illuvium_Test : ModuleRules
{
	public Illuvium_Test(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
