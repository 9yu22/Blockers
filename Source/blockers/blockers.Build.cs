// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;

using UnrealBuildTool;

public class blockers : ModuleRules
{
	public blockers(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "Sockets", "Networking", "UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent" });
    }
}
