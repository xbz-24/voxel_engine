TEST_CASE("public engine api accepts simple cxx configuration")
{
	voxel::EngineConfig config = voxel::EngineConfig::MinecraftDemo()
		.WithWindow(voxel::Window("API Smoke", 1024, 576).EnableVSync())
		.WithWorld(voxel::World()
			.WithSizeChunks(6)
			.SetBlock(0, 64, 0, voxel::Stone)
			.FillBox(-2, 63, -2, 2, 63, 2, voxel::Grass)
			.AddFloor(60, 4, voxel::Dirt)
			.AddPlatform(61, 2, voxel::OakPlanks)
			.AddColumn(3, 62, 3, 3, voxel::OakLog)
			.AddLamp(3, 65, 3))
		.WithCamera({ 20.0f, 70.0f, 20.0f }, { 0.0f, 64.0f, 0.0f })
		.UseVulkan()
		.WithRenderDistanceChunks(3);

	voxel::Engine engine{ config };
	engine.RequestStop();

	CHECK(config.window.title == "API Smoke");
	CHECK(config.window.width == 1024);
	CHECK(config.window.height == 576);
	CHECK(config.window.vsync);
	CHECK(config.world.size_chunks == 6);
	CHECK(config.camera.has_custom_view);
	CHECK(config.camera.position.y == doctest::Approx(70.0f));
	REQUIRE(config.world.edits.size() == 6);
	CHECK(config.world.edits[0].block == voxel::Stone);
	CHECK(config.world.edits[1].kind == voxel::WorldEdit::Kind::FillBox);
	CHECK(config.world.edits[2].box.minimum.x == -4);
	CHECK(config.world.edits[2].box.maximum.z == 4);
	CHECK(config.world.edits[4].box.maximum.y == 64);
	CHECK(config.world.edits[5].block == voxel::SeaLantern);
}

TEST_CASE("public run helper is available without exposing internals")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithWindow("Window API Smoke", 800, 600)
		.UseOpenGLCompatibility()
		.HideDebugOverlay();

	CHECK(!config.window.fullscreen);
	CHECK(config.window.title == "Window API Smoke");
	CHECK(config.window.width == 800);
	CHECK(config.window.height == 600);
	CHECK(config.graphics_api == voxel::GraphicsApi::OpenGLCompatibility);
	CHECK(!config.show_debug_overlay);
}

TEST_CASE("public engine config separates startup and runtime tuning views")
{
	const voxel::VoxelRenderStyle render_style = voxel::VoxelRenderStyle{}
		.WithSunDirection({ -0.25f, 0.90f, -0.35f })
		.WithSunColor({ 1.40f, 1.10f, 0.82f })
		.WithSunIntensity(1.25f)
		.WithSkyColors({ 0.60f, 0.72f, 0.80f }, { 0.24f, 0.46f, 0.78f })
		.WithExposure(1.10f)
		.WithFogRange(90.0f, 360.0f)
		.WithFogStrength(0.58f)
		.WithClouds(0.36f, 0.68f, 0.024f)
		.WithSurfaceDetail(1.2f)
		.WithWaterReflections(1.4f)
		.WithShadowStrength(0.9f)
		.WithSpecularStrength(1.3f);
	const voxel::EngineRuntimeTuning tuning{
		4,
		false,
		false
	};
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithWindow("Split Config", 960, 540)
		.WithVoxelRenderStyle(render_style)
		.WithRuntimeTuning(tuning);

	const voxel::EngineStartupConfig startup = config.StartupConfig();
	const voxel::EngineRuntimeTuning runtime = config.RuntimeTuning();

	CHECK(startup.window.title == "Split Config");
	CHECK(startup.window.width == 960);
	CHECK(startup.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(startup.voxel_render_style.sun_intensity == doctest::Approx(1.25f));
	CHECK(startup.voxel_render_style.sky_zenith_color.z == doctest::Approx(0.78f));
	CHECK(startup.voxel_render_style.fog_start_distance == doctest::Approx(90.0f));
	CHECK(startup.voxel_render_style.fog_strength == doctest::Approx(0.58f));
	CHECK(startup.voxel_render_style.cloud_coverage == doctest::Approx(0.36f));
	CHECK(startup.voxel_render_style.cloud_density == doctest::Approx(0.68f));
	CHECK(startup.voxel_render_style.water_reflection_strength == doctest::Approx(1.4f));
	CHECK(startup.voxel_render_style.specular_strength == doctest::Approx(1.3f));
	CHECK(runtime.render_distance_chunks == 4);
	CHECK(!runtime.show_debug_overlay);
	CHECK(!runtime.enable_settings_menu);
}

TEST_CASE("public config can disable demo settings menu controls")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.DisableSettingsMenu();

	CHECK(!config.enable_settings_menu);
	CHECK(!config.RuntimeTuning().enable_settings_menu);
}

TEST_CASE("public config exposes logging sinks and stop requests")
{
	const voxel::LogSettings logging = voxel::LogSettings{}
		.WithMinimumLevel(voxel::LogLevel::Warning)
		.DisableConsole()
		.WriteToFile("custom-engine.log");

	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithLogging(logging)
		.WithLogLevel(voxel::LogLevel::Error)
		.EnableConsoleLogging()
		.DisableFileLogging();

	voxel::Engine engine{ config };
	engine.RequestStop();

	CHECK(config.logging.minimum_level == voxel::LogLevel::Error);
	CHECK(config.logging.console_enabled);
	CHECK(!config.logging.file_output_enabled);
	CHECK(config.logging.file_output_path.empty());
}

TEST_CASE("public engine exposes nonblocking frame lifecycle")
{
	const voxel::EngineConfig invalid_config = voxel::EngineConfig::Default()
		.WithWindow(voxel::Window("Invalid", 0, 600));

	voxel::Engine engine{ invalid_config };

	CHECK(!engine.IsRunning());
	const voxel::EngineStartResult start_result = engine.StartDetailed();
	CHECK(!start_result);
	CHECK(start_result.failure == voxel::EngineStartFailure::InvalidConfiguration);
	CHECK(start_result.message == "EngineConfig is invalid");
	REQUIRE(start_result.issues.size() == 1);
	CHECK(start_result.issues.front() == "window.width must be greater than zero");
	CHECK(!engine.Start());
	CHECK(!engine.Step());
	engine.Shutdown();
	CHECK(!engine.IsRunning());
}

TEST_CASE("public window config exposes display and startup options")
{
	const voxel::WindowConfig config = voxel::Fullscreen("Display API")
		.UseMonitor(1)
		.WithRefreshRate(144)
		.SetResizable(false)
		.EnableHighDpiFramebuffer(false)
		.CaptureCursorOnStart()
		.EnableVSync();

	CHECK(config.fullscreen);
	CHECK(config.monitor_index == 1);
	CHECK(config.refresh_rate_hertz == 144);
	CHECK(!config.resizable);
	CHECK(!config.high_dpi_framebuffer);
	CHECK(config.capture_cursor_on_start);
	CHECK(config.vsync);
}

TEST_CASE("public top-level helpers expose the shortest startup path")
{
	const voxel::EngineConfig config = voxel::DesertDemo();
	const voxel::EngineConfig aqua = voxel::AquaModelDemo();
	const voxel::EngineConfig sponza = voxel::SponzaAtriumDemo();
	const voxel::EngineConfig legacy_alias = voxel::MinecraftDemo();
	const voxel::SdkVersion version = voxel::Version();
	const voxel::SdkFeatures features = voxel::Features();

	CHECK(config.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(config.world.size_chunks == 20);
	CHECK(config.render_distance_chunks == 16);
	CHECK(config.window.title == "Voxel Engine - Crystal Jungle");
	CHECK(config.demo_scene == voxel::DemoScene::HyperrealDesert);
	CHECK(aqua.demo_scene == voxel::DemoScene::AquaModel);
	CHECK(sponza.demo_scene == voxel::DemoScene::SponzaAtrium);
	CHECK(aqua.window.title == "Voxel Engine - Aqua Model Preview");
	CHECK(sponza.window.title == "Voxel Engine - Sponza Atrium Preview");
	CHECK(legacy_alias.window.title == config.window.title);
	CHECK(version.major == 0);
	CHECK(version.minor == 3);
	CHECK(features.vulkan_by_default);
	CHECK(features.world_config_serialization);
	CHECK(features.asset_search_roots);
	CHECK(!features.asset_catalog_runtime_loading);
	CHECK(!features.material_library_runtime_binding);
	CHECK(features.runtime_update_callback);
	CHECK(features.scene_graph_authoring);
	CHECK(!features.scene_graph_runtime_rendering);
	CHECK(features.embeddable_frame_loop);
	CHECK(features.configurable_voxel_render_style);
	CHECK(features.animated_voxel_atmosphere);
	CHECK(!features.directx12_runtime_backend);
}
