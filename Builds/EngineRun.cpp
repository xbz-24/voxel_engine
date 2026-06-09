#include "Engine.h"

#include "EngineRuntime.h"

/** Starts the engine runtime and returns a process-style status code. */
int Engine::Run()
{
	ve::engine::EngineRuntime runtime{ *this };
	return runtime.Execute();
}
