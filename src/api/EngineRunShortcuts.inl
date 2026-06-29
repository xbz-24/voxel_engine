	int Run(EngineConfig config)
	{
		Engine engine{ std::move(config) };
		return engine.Run();
	}

	int Run(WorldConfig world)
	{
		return Run(DesertDemo().WithWorld(std::move(world)));
	}

	int RunScene(WorldConfig scene)
	{
		return Run(std::move(scene));
	}

	int RunDesertDemo()
	{
		return Run(DesertDemo());
	}

	int RunAquaModelDemo()
	{
		return Run(AquaModelDemo());
	}

	int RunSponzaAtriumDemo()
	{
		return Run(SponzaAtriumDemo());
	}

	int RunMinecraftDemo()
	{
		return RunDesertDemo();
	}

	int RunWorldEditDemo()
	{
		return Run(WorldEditDemo());
	}

	int RunShowcaseDemo()
	{
		return Run(ShowcaseDemo());
	}

	int RunArcadeSnakeDemo()
	{
		return Run(ArcadeSnakeDemo());
	}

	int RunArcadePaddleDemo()
	{
		return Run(ArcadePaddleDemo());
	}

	int RunArcadeBlocksDemo()
	{
		return Run(ArcadeBlocksDemo());
	}

	int RunArcadeInvadersDemo()
	{
		return Run(ArcadeInvadersDemo());
	}

	int RunArcadeMazeDemo()
	{
		return Run(ArcadeMazeDemo());
	}
}
