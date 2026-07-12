	namespace
	{
		[[nodiscard]] DemoScene ToPublicDemoScene(ve::rendering::VulkanMinecraftDemoPreset preset) noexcept
		{
			switch (preset)
			{
			case ve::rendering::VulkanMinecraftDemoPreset::AquaModel:
				return DemoScene::AquaModel;
			case ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium:
				return DemoScene::SponzaAtrium;
			case ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert:
			default:
				return DemoScene::HyperrealDesert;
			}
		}

		[[nodiscard]] Vec3 ToPublicVec3(glm::vec3 value) noexcept
		{
			return { value.x, value.y, value.z };
		}

		[[nodiscard]] VoxelRenderStyle VoxelRenderStyleFor(
			ve::rendering::VulkanMinecraftDemoPreset preset) noexcept
		{
			VoxelRenderStyle style{};
			switch (preset)
			{
			case ve::rendering::VulkanMinecraftDemoPreset::AquaModel:
				return style
					.WithSunDirection({ -0.30f, 0.72f, -0.62f })
					.WithSunColor({ 0.86f, 1.02f, 1.20f })
					.WithSunIntensity(1.08f)
					.WithSkyColors({ 0.38f, 0.62f, 0.76f }, { 0.15f, 0.34f, 0.62f })
					.WithExposure(1.04f)
					.WithFogRange(120.0f, 390.0f)
					.WithFogStrength(0.52f)
					.WithClouds(0.28f, 0.48f, 0.014f)
					.WithSurfaceDetail(0.82f)
					.WithWaterReflections(1.30f);
			case ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium:
				return style
					.WithSunDirection({ -0.55f, 0.62f, -0.56f })
					.WithSunColor({ 1.30f, 0.98f, 0.72f })
					.WithSunIntensity(1.14f)
					.WithSkyColors({ 0.74f, 0.60f, 0.48f }, { 0.42f, 0.55f, 0.72f })
					.WithExposure(0.98f)
					.WithFogRange(140.0f, 430.0f)
					.WithFogStrength(0.34f)
					.WithClouds(0.58f, 0.66f, 0.010f)
					.WithSpecularStrength(0.88f);
			case ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert:
			default:
				return style;
			}
		}

		[[nodiscard]] EngineConfig VulkanDemoConfig(ve::rendering::VulkanMinecraftDemoPreset preset)
		{
			const ve::rendering::VulkanMinecraftDemoProfile profile =
				ve::rendering::VulkanMinecraftDemoProfileFor(preset);
			EngineConfig config{};
			config.window = WindowConfig::Windowed(std::string{ profile.window_title });
			config.world = WorldConfig::SizeChunks(profile.world_size_chunks);
			config.camera = CameraConfig::At(
				ToPublicVec3(profile.camera_position),
				ToPublicVec3(profile.camera_target));
			config.graphics_api = GraphicsApi::Vulkan;
			config.demo_scene = ToPublicDemoScene(profile.preset);
			config.voxel_render_style = VoxelRenderStyleFor(profile.preset);
			config.render_distance_chunks = profile.render_distance_chunks;
			config.show_debug_overlay = true;
			return config;
		}
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
