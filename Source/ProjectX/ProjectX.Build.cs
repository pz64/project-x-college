// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectX : ModuleRules
{
	public ProjectX(ReadOnlyTargetRules Target) : base(Target)
	{
        bFasterWithoutUnity = true;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "GameplayTasks" , "UMG", "Slate", "SlateCore" });
	}
}
