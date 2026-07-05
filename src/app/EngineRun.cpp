#include "Engine.h"

#include "EngineRuntime.h"

/** Starts the engine runtime and returns a process-style status code. */
int EngineApplication::Run()
{
	// TODO: Support an externally driven Tick/Present mode once EngineRuntime owns a nonblocking lifecycle.
	stop_requested_.store(false, std::memory_order_relaxed);
	ve::engine::EngineRuntime runtime{ *this };
	return runtime.Execute();
}

void EngineApplication::RequestStop() noexcept
{
	stop_requested_.store(true, std::memory_order_relaxed);
}

bool EngineApplication::IsStopRequested() const noexcept
{
	return stop_requested_.load(std::memory_order_relaxed);
}
