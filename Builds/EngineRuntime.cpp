#include "EngineRuntime.h"

#include "Logger.h"

namespace
{
	constexpr int kDefaultWorldSizeChunks = 10;
}

namespace ve::engine
{
	/** Stores the engine facade used by runtime systems. */
	EngineRuntime::EngineRuntime(Engine& engine) noexcept
		: engine_(engine)
	{
	}

	/** Initializes, runs, and shuts down the runtime. */
	int EngineRuntime::Execute()
	{
		if (!Initialize()) return -1;
		RunMainLoop();
		Shutdown();
		return 0;
	}

	/** Creates the window and every runtime system needed by the frame loop. */
	bool EngineRuntime::Initialize()
	{
		callback_context_.isSettingsMenuOpen = &engine_._runtimeSettings.isSettingsMenuOpen;
		if (!engine_.InitializeWindow(window_)) return false;
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility) engine_.ConfigureOpenGLState();
		PrepareAssetsAndLogging();
		CreateRuntimeSystems();
		return true;
	}

	/** Resolves asset paths and configures logger sinks. */
	void EngineRuntime::PrepareAssetsAndLogging()
	{
		asset_paths_ = ve::assets::ResolveFromSourceFile(engine_._applicationSourceFilePath);
		engine_.ConfigureRuntimeLogging(asset_paths_);
	}

	/** Creates model, callbacks, editor UI, and view resources. */
	void EngineRuntime::CreateRuntimeSystems()
	{
		model_ = std::make_unique<GameModel>(kDefaultWorldSizeChunks);
		callback_context_.camera = &model_->MutableCamera();
		engine_.ConfigureCallbacks(window_, callback_context_);
		editor_controller_.Initialize(window_, engine_._runtimeSettings);
		view_ = std::make_unique<GameView>(asset_paths_);
	}

	/** Runs frames until the window asks to close. */
	void EngineRuntime::RunMainLoop()
	{
		while (!window_.ShouldClose()) RunFrame();
	}

	/** Releases runtime resources and reports shutdown. */
	void EngineRuntime::Shutdown()
	{
		editor_controller_.Shutdown();
		engine_.ReleaseRenderCaches();
		VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
	}
}
