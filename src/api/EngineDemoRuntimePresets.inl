	EngineConfig EngineConfig::DesertDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine - Crystal Jungle");
		config.world = WorldConfig::SizeChunks(20);
		config.camera = CameraConfig::At({ 154.0f, 82.0f, 292.0f }, { 160.0f, 52.0f, 86.0f });
		config.graphics_api = GraphicsApi::Vulkan;
		config.demo_scene = DemoScene::HyperrealDesert;
		config.render_distance_chunks = 16;
		config.show_debug_overlay = true;
		return config;
	}

	EngineConfig EngineConfig::AquaModelDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine - Aqua Model Preview");
		config.world = WorldConfig::SizeChunks(16);
		config.camera = CameraConfig::At({ 120.0f, 82.0f, 168.0f }, { 92.0f, 58.0f, 92.0f });
		config.graphics_api = GraphicsApi::Vulkan;
		config.demo_scene = DemoScene::AquaModel;
		config.render_distance_chunks = 12;
		config.show_debug_overlay = true;
		return config;
	}

	EngineConfig EngineConfig::SponzaAtriumDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine - Sponza Atrium Preview");
		config.world = WorldConfig::SizeChunks(20);
		config.camera = CameraConfig::At({ 176.0f, 90.0f, 210.0f }, { 128.0f, 58.0f, 116.0f });
		config.graphics_api = GraphicsApi::Vulkan;
		config.demo_scene = DemoScene::SponzaAtrium;
		config.render_distance_chunks = 16;
		config.show_debug_overlay = true;
		return config;
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
