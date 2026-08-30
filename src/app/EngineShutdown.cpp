#include "Engine.h"

#include "EngineRuntime.h"

#include <utility>

void EngineApplication::StopRuntime(
	std::unique_ptr<ve::engine::EngineRuntime> runtime) noexcept
{
	phase_ = ve::engine::EngineApplicationPhase::Stopping;
	if (runtime != nullptr) runtime->Shutdown();
	runtime.reset();
	shutdown_requested_ = false;
	phase_ = ve::engine::EngineApplicationPhase::Idle;
}

void EngineApplication::StopOwnedRuntime() noexcept
{
	StopRuntime(std::move(runtime_));
}

void EngineApplication::Shutdown() noexcept
{
	using ve::engine::EngineApplicationPhase;
	if (phase_ == EngineApplicationPhase::Starting)
	{
		shutdown_requested_ = true;
		return;
	}
	if (phase_ == EngineApplicationPhase::Stepping)
	{
		shutdown_requested_ = true;
		if (runtime_ != nullptr) runtime_->RequestStop();
		return;
	}
	if (phase_ == EngineApplicationPhase::Running) StopOwnedRuntime();
}

void EngineApplication::RequestStop() noexcept
{
	using ve::engine::EngineApplicationPhase;
	if (phase_ == EngineApplicationPhase::Starting)
	{
		shutdown_requested_ = true;
		return;
	}
	if ((phase_ == EngineApplicationPhase::Running ||
		phase_ == EngineApplicationPhase::Stepping) && runtime_ != nullptr)
	{
		runtime_->RequestStop();
	}
}

bool EngineApplication::IsRunning() const noexcept
{
	using ve::engine::EngineApplicationPhase;
	return runtime_ != nullptr && runtime_->IsActive() && !shutdown_requested_ &&
		(phase_ == EngineApplicationPhase::Running ||
			phase_ == EngineApplicationPhase::Stepping);
}
