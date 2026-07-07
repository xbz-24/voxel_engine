	EngineConfig EngineConfig::Default()
	{
		return {};
	}

	EngineStartupConfig EngineConfig::StartupConfig() const
	{
		return EngineStartupConfig{
			window,
			world,
			assets,
			materials,
			scene_graph,
			camera,
			logging,
			graphics_api,
			demo_scene
		};
	}

	EngineRuntimeTuning EngineConfig::RuntimeTuning() const noexcept
	{
		return EngineRuntimeTuning{
			render_distance_chunks,
			show_debug_overlay,
			enable_settings_menu
		};
	}

	EngineConfig& EngineConfig::WithWindow(WindowConfig value) noexcept
	{
		window = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithWindow(std::string title, int width, int height)
	{
		return WithWindow(Window(std::move(title), width, height));
	}

	EngineConfig& EngineConfig::WithWorld(WorldConfig value) noexcept
	{
		world = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithScene(WorldConfig value) noexcept
	{
		return WithWorld(std::move(value));
	}

	EngineConfig& EngineConfig::WithAssets(AssetCatalog value) noexcept
	{
		assets = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithMaterials(MaterialLibrary value) noexcept
	{
		materials = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithSceneGraph(SceneGraph value) noexcept
	{
		scene_graph = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithCamera(CameraConfig value) noexcept
	{
		camera = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithCamera(Vec3 position, Vec3 look_at) noexcept
	{
		return WithCamera(Camera(position, look_at));
	}

	EngineConfig& EngineConfig::WithLogging(LogSettings value) noexcept
	{
		logging = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithLogLevel(LogLevel value) noexcept
	{
		logging.minimum_level = value;
		return *this;
	}

	EngineConfig& EngineConfig::LogToFile(std::string path)
	{
		logging.WriteToFile(std::move(path));
		return *this;
	}

	EngineConfig& EngineConfig::DisableFileLogging() noexcept
	{
		logging.DisableFileOutput();
		return *this;
	}

	EngineConfig& EngineConfig::EnableConsoleLogging(bool enabled) noexcept
	{
		logging.EnableConsole(enabled);
		return *this;
	}
