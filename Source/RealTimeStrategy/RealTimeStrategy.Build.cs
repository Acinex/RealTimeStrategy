namespace UnrealBuildTool.Rules
{
	public class RealTimeStrategy : ModuleRules
	{
		public RealTimeStrategy(ReadOnlyTargetRules Target)
			: base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(
				new[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"AIModule",
					"UMG",
					"Slate",
					"SlateCore",
					"RHI",
					"RenderCore",
					"Landscape",
					"GameplayTags",
					"GameplayTasks",
					"NavigationSystem",
					"GameplayAbilities",
					"DeveloperSettings"
				});
			
			PrivateDependencyModuleNames.AddRange(new string[] {"Niagara"});

			// Setup Gameplay Debugger.
			if (Target.bBuildDeveloperTools || (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test))
			{
				PrivateDependencyModuleNames.Add("GameplayDebugger");
				PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
			}
		}
	}
}