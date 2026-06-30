namespace ve::engine
{
	/** Stores the engine facade used by runtime systems. */
	EngineRuntime::EngineRuntime(EngineApplication& engine) noexcept
		: engine_(engine),
		  window_(engine.create_info_.window)
	{
	}

	/** Initializes, runs, and shuts down the runtime. */
	int EngineRuntime::Execute()
	{
		if (!Initialize())
		{
			Shutdown();
			return -1;
		}
		RunMainLoop();
		Shutdown();
		return 0;
	}

	/** Creates the window and every runtime system needed by the frame loop. */
	bool EngineRuntime::Initialize()
	{
		callback_context_.isSettingsMenuOpen = &engine_._runtimeSettings.isSettingsMenuOpen;
		PrepareAssetsAndLogging();
		if (!engine_.InitializeWindow(window_)) return false;
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility) engine_.ConfigureOpenGLState();
		return CreateRuntimeSystems();
	}

	/** Resolves asset paths and configures logger sinks. */
	void EngineRuntime::PrepareAssetsAndLogging()
	{
		ve::assets::AssetPathResolveOptions asset_path_options;
		asset_path_options.search_roots = engine_.create_info_.asset_search_roots;
		asset_paths_ = ve::assets::Resolve(asset_path_options);
		ve::log::SetCallback(engine_.create_info_.on_log);
		engine_.ConfigureRuntimeLogging(asset_paths_);
	}

