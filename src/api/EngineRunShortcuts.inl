	int Run(EngineConfig config)
	{
		Engine engine{ std::move(config) };
		return engine.Run();
	}

	int Run(WorldConfig world)
	{
		return Run(EngineConfig::Default().WithWorld(std::move(world)));
	}

	int RunScene(WorldConfig scene)
	{
		return Run(std::move(scene));
	}
}
