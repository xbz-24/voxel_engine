	CHECK(startup.voxel_render_style.fog_start_distance == doctest::Approx(90.0f));
	CHECK(startup.voxel_render_style.fog_strength == doctest::Approx(0.58f));
	CHECK(startup.voxel_render_style.cloud_coverage == doctest::Approx(0.36f));
	CHECK(startup.voxel_render_style.cloud_density == doctest::Approx(0.68f));
	CHECK(startup.voxel_render_style.water_reflection_strength == doctest::Approx(1.4f));
	CHECK(startup.voxel_render_style.specular_strength == doctest::Approx(1.3f));
	CHECK(startup.voxel_render_style.ambient_occlusion_strength == doctest::Approx(1.1f));
	CHECK(startup.voxel_render_style.aerial_perspective_strength == doctest::Approx(0.8f));
	CHECK(startup.voxel_render_style.subsurface_scattering_strength == doctest::Approx(1.4f));
	CHECK(startup.voxel_render_style.cloud_light_absorption == doctest::Approx(0.7f));
	CHECK(startup.voxel_render_style.directional_shadow_strength == doctest::Approx(1.15f));
	CHECK(startup.voxel_render_style.directional_shadow_softness == doctest::Approx(0.75f));
	CHECK(startup.voxel_render_style.directional_shadow_distance == doctest::Approx(196.0f));
	CHECK(runtime.render_distance_chunks == 4);
	CHECK(!runtime.show_debug_overlay);
	CHECK(!runtime.enable_settings_menu);
}

TEST_CASE("public config can disable runtime settings controls")
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
