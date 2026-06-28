#include "Engine.h"

#include "EngineRuntime.h"

/** Starts the engine runtime and returns a process-style status code. */
int EngineApplication::Run()
{
	// TODO: Support an externally driven run mode where callers own the app loop and pump EngineRuntime manually.
	ve::engine::EngineRuntime runtime{ *this };
	return runtime.Execute();
}
