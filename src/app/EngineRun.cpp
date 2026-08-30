#include "Engine.h"

/** Starts the engine runtime and returns a process-style status code. */
int EngineApplication::Run()
{
	using ve::engine::EngineApplicationPhase;
	if (phase_ == EngineApplicationPhase::Starting ||
		phase_ == EngineApplicationPhase::Stepping ||
		phase_ == EngineApplicationPhase::Stopping) return -1;
	if (!Start())
	{
		return -1;
	}
	while (Step()) {}
	return 0;
}
