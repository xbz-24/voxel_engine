#include "EngineRuntime.h"

#include "Logger.h"

#include <utility>

namespace ve::engine
{
	EngineRuntime::EngineRuntime(RuntimeHostConfiguration configuration,
		std::unique_ptr<IRuntimeModule> module)
		: configuration_(std::move(configuration)),
		  module_(std::move(module)),
		  window_(configuration_.window)
	{
	}

	EngineStartupResult EngineRuntime::Start()
	{
		const EngineStartupResult startup_result = Initialize();
		if (!startup_result)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, startup_result.message);
			Shutdown();
		}
		return startup_result;
	}

	bool EngineRuntime::Step()
	{
		if (!ShouldContinue()) return false;
		RunFrame();
		return ShouldContinue();
	}

	/** Initializes, runs, and shuts down the runtime. */
	int EngineRuntime::Execute()
	{
		if (!Start()) return -1;
		RunMainLoop();
		Shutdown();
		return 0;
	}

	/** Creates the window and every runtime system needed by the frame loop. */
	EngineStartupResult EngineRuntime::Initialize()
	{
		PrepareAssetsAndLogging();
		const EngineStartupResult window_result = InitializeWindow();
		if (!window_result) return window_result;
		if (module_ == nullptr)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::RuntimeModuleUnavailable,
				"Engine runtime content module was not created");
		}
		RuntimeModuleContext context{ window_, asset_paths_, frame_timer_ };
		return module_->Initialize(context);
	}
}
