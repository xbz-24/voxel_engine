	}

	EngineConfig EngineConfig::DesertDemo()
	{
		return VulkanDemoConfig(ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert);
	}

	EngineConfig EngineConfig::AquaModelDemo()
	{
		return VulkanDemoConfig(ve::rendering::VulkanMinecraftDemoPreset::AquaModel);
	}

	EngineConfig EngineConfig::SponzaAtriumDemo()
	{
		return VulkanDemoConfig(ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium);
	}

	EngineConfig EngineConfig::MinecraftDemo()
	{
		return DesertDemo();
	}

	EngineConfig EngineConfig::WorldEditDemo()
	{
		return MinecraftDemo()
			.WithWindow(Window("Voxel API - World Edit", 1280, 720).EnableVSync())
			.WithScene(WorldEditDemoSceneBuilder{}.Build())
			.WithCamera({ 18.0f, 72.0f, 18.0f }, { 0.0f, 64.0f, 0.0f })
			.WithRenderDistanceChunks(3);
	}

	EngineConfig EngineConfig::ShowcaseDemo()
	{
		return MinecraftDemo()
			.WithWindow(Window("Voxel API - Showcase", 1600, 900).EnableVSync())
			.WithScene(ShowcaseDemoSceneBuilder{}.Build())
			.WithCamera({ 24.0f, 76.0f, 28.0f }, { 0.0f, 64.0f, 0.0f })
			.WithRenderDistanceChunks(4)
			.ShowDebugOverlay(false);
	}
