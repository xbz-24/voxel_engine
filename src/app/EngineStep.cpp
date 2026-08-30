#include "Engine.h"

#include "EngineRuntime.h"

bool EngineApplication::Step()
{
	using ve::engine::EngineApplicationPhase;
	if (phase_ != EngineApplicationPhase::Running || runtime_ == nullptr) return false;
	phase_ = EngineApplicationPhase::Stepping;
	try
	{
		const bool should_request_next_frame = runtime_->Step();
		if (!should_request_next_frame || shutdown_requested_)
		{
			StopOwnedRuntime();
			return false;
		}
		phase_ = EngineApplicationPhase::Running;
		return true;
	}
	catch (...)
	{
		StopOwnedRuntime();
		throw;
	}
}
