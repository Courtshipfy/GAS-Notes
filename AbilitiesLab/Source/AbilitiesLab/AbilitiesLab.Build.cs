// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AbilitiesLab : ModuleRules
{
	public AbilitiesLab(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// 基础模块 + GAS 必需的三个模块（对应教程「第 0 步：在 Build.cs 中引入模块」）
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore",
			"GameplayAbilities", "GameplayTags", "GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
