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
		return !window_.ShouldClose() && !IsStopRequested();
	}

	/** Releases runtime resources and reports shutdown. */
	void EngineRuntime::Shutdown()
	{
		if (module_ != nullptr) module_->Shutdown();
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
