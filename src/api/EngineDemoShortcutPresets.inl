	EngineConfig EngineConfig::ArcadeSnakeDemo()
	{
		return ArcadeConfig("Voxel Arcade - Snake Trail", ArcadeSnakeSceneBuilder{}.Build());
	}

	EngineConfig EngineConfig::ArcadePaddleDemo()
	{
		return ArcadeConfig("Voxel Arcade - Paddle Duel", ArcadePaddleSceneBuilder{}.Build());
	}

	EngineConfig EngineConfig::ArcadeBlocksDemo()
	{
		return ArcadeConfig("Voxel Arcade - Falling Blocks", ArcadeBlocksSceneBuilder{}.Build());
	}

	EngineConfig EngineConfig::ArcadeInvadersDemo()
	{
		return ArcadeConfig("Voxel Arcade - Star Defenders", ArcadeInvadersSceneBuilder{}.Build());
	}

	EngineConfig EngineConfig::ArcadeMazeDemo()
	{
		return ArcadeConfig("Voxel Arcade - Neon Maze", ArcadeMazeSceneBuilder{}.Build());
	}

	EngineConfig DesertDemo()
	{
		return EngineConfig::DesertDemo();
	}

	EngineConfig AquaModelDemo()
	{
		return EngineConfig::AquaModelDemo();
	}

	EngineConfig SponzaAtriumDemo()
	{
		return EngineConfig::SponzaAtriumDemo();
	}

	EngineConfig MinecraftDemo()
	{
		return DesertDemo();
	}

	EngineConfig WorldEditDemo()
	{
		return EngineConfig::WorldEditDemo();
	}

	EngineConfig ShowcaseDemo()
	{
		return EngineConfig::ShowcaseDemo();
	}

	EngineConfig ArcadeSnakeDemo()
	{
		return EngineConfig::ArcadeSnakeDemo();
	}

	EngineConfig ArcadePaddleDemo()
	{
		return EngineConfig::ArcadePaddleDemo();
	}

	EngineConfig ArcadeBlocksDemo()
	{
		return EngineConfig::ArcadeBlocksDemo();
	}

	EngineConfig ArcadeInvadersDemo()
	{
		return EngineConfig::ArcadeInvadersDemo();
	}

	EngineConfig ArcadeMazeDemo()
	{
		return EngineConfig::ArcadeMazeDemo();
	}

	EngineConfig DefaultConfig()
	{
		return EngineConfig::Default();
	}
}
