#include "EngineRuntime.h"

#include "Logger.h"

namespace ve::engine
{
/** Runs frames until the window asks to close. */
	void EngineRuntime::RunMainLoop()
	{
		while (Step()) {}
	}

	bool EngineRuntime::ShouldContinue() const noexcept
	{
		return !window_.ShouldClose() && !engine_.IsStopRequested();
	}

	/** Releases runtime resources and reports shutdown. */
	void EngineRuntime::Shutdown()
	{
		editor_controller_.Shutdown();
		model_.reset();
		if (!render_driver_)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Engine, "No runtime render driver to release");
			VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
			return;
		}
		render_driver_->Shutdown();
		render_driver_.reset();
		VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
	}

}
