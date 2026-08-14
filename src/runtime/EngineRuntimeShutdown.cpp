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
		return module_lifecycle_.IsActive() && window_.GetNativeWindow() != nullptr &&
			!window_.ShouldClose() && !IsStopRequested();
	}

	/** Releases runtime resources and reports shutdown. */
	void EngineRuntime::Shutdown() noexcept
	{
		window_.MakeGraphicsContextCurrent();
		module_lifecycle_.Shutdown();
		window_.Shutdown();
		ReleaseLoggingSession();
	}

	void EngineRuntime::RequestStop() noexcept
	{
		stop_requested_.store(true, std::memory_order_relaxed);
	}

	bool EngineRuntime::IsStopRequested() const noexcept
	{
		return stop_requested_.load(std::memory_order_relaxed);
	}
}
