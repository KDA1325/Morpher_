// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class N_Graduation_project : ModuleRules
{
	public N_Graduation_project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "NavigationSystem", "AIModule", "GameplayTasks"});
	}
}
