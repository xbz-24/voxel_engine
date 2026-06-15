#include "EngineRuntime.h"

#include "Logger.h"
#include "OpenGLRenderView.h"
#include "RenderViewFactory.h"

#include <cassert>

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
		if (!engine_.InitializeWindow(window_)) return false;
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility) engine_.ConfigureOpenGLState();
		PrepareAssetsAndLogging();
		return CreateRuntimeSystems();
	}

	/** Resolves asset paths and configures logger sinks. */
	void EngineRuntime::PrepareAssetsAndLogging()
	{
		asset_paths_ = ve::assets::ResolveFromSourceFile(engine_._applicationSourceFilePath);
		engine_.ConfigureRuntimeLogging(asset_paths_);
	}

	/** Creates model, callbacks, editor UI, and view resources. */
	bool EngineRuntime::CreateRuntimeSystems()
	{
		model_ = std::make_unique<GameModel>(kDefaultWorldSizeChunks);
		callback_context_.camera = &model_->MutableCamera();
		engine_.ConfigureCallbacks(window_, callback_context_);
		editor_controller_.Initialize(window_, engine_._runtimeSettings);
		view_ = RenderViewFactory::Create({ window_.GraphicsApi(), &asset_paths_, nullptr });
		if (view_ == nullptr)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Render view creation failed");
			return false;
		}
		if (TryRenderViewCast<OpenGLRenderView>(*view_) == nullptr)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Legacy world renderer still requires an OpenGL render view");
			return false;
		}
		return true;
	}

	/** Returns the OpenGL compatibility view after startup validation. */
	OpenGLRenderView& EngineRuntime::LegacyOpenGLView() noexcept
	{
		OpenGLRenderView* legacy_view = view_ ? TryRenderViewCast<OpenGLRenderView>(*view_) : nullptr;
		assert(legacy_view != nullptr);
		return *legacy_view;
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
