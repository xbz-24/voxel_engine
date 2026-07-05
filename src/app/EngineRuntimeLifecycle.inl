namespace ve::engine
{
	/** Stores the engine facade used by runtime systems. */
	EngineRuntime::EngineRuntime(EngineApplication& engine) noexcept
		: engine_(engine),
		  window_(engine.CreateInfo().window)
	{
	}

	/** Initializes, runs, and shuts down the runtime. */
	int EngineRuntime::Execute()
	{
		const EngineStartupResult startup_result = Initialize();
		if (!startup_result)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, startup_result.message);
			Shutdown();
			return -1;
		}
		RunMainLoop();
		Shutdown();
		return 0;
	}

	/** Creates the window and every runtime system needed by the frame loop. */
	EngineStartupResult EngineRuntime::Initialize()
	{
		PrepareAssetsAndLogging();
		const EngineStartupResult window_result = engine_.InitializeWindow(window_);
		if (!window_result) return window_result;
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility) engine_.ConfigureOpenGLState();
		return CreateRuntimeSystems();
	}

	/** Resolves asset paths and configures logger sinks. */
	void EngineRuntime::PrepareAssetsAndLogging()
	{
		ve::assets::AssetPathResolveOptions asset_path_options;
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		asset_path_options.search_roots = create_info.asset_search_roots;
		asset_paths_ = ve::assets::Resolve(asset_path_options);
		ve::log::SetCallback(create_info.on_log);
		engine_.ConfigureRuntimeLogging(asset_paths_);
	}

