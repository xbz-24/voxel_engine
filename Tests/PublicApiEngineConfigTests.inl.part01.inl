TEST_CASE("public engine api accepts simple cxx configuration")
{
	voxel::EngineConfig config = voxel::EngineConfig::Default()
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
		.WithSpecularStrength(1.3f)
		.WithAmbientOcclusion(1.1f)
		.WithAerialPerspective(0.8f)
		.WithSubsurfaceScattering(1.4f)
		.WithCloudLightAbsorption(0.7f)
		.WithDirectionalShadows(1.15f, 0.75f, 196.0f);
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
