// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Dark : ModuleRules
{
	public Dark(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"GameplayAbilities", 
			"GameplayTags", 
			"GameplayTasks",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"NetCore", 
			"ModularGameplay",
		});

		SetupGameplayDebuggerSupport(Target);
		SetupIrisSupport(Target);
	}
}
