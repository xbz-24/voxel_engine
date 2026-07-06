#include "Engine.h"

#include "EngineRuntime.h"

bool EngineApplication::Start()
{
	if (!HasValidCreateInfo())
	{
		return false;
	}
	if (runtime_ != nullptr)
	{
		return true;
	}
	stop_requested_.store(false, std::memory_order_relaxed);
	auto runtime = std::make_unique<ve::engine::EngineRuntime>(*this);
	if (!runtime->Start())
	{
		return false;
	}
	runtime_ = std::move(runtime);
	return true;
}

bool EngineApplication::Step()
{
	if (runtime_ == nullptr)
	{
		return false;
	}
	const bool should_request_next_frame = runtime_->Step();
	if (!should_request_next_frame)
	{
		Shutdown();
	}
	return should_request_next_frame;
}

void EngineApplication::Shutdown() noexcept
{
	if (runtime_ == nullptr)
	{
		return;
	}
	runtime_->Shutdown();
	runtime_.reset();
}

/** Starts the engine runtime and returns a process-style status code. */
int EngineApplication::Run()
{
	if (!Start())
	{
		return -1;
	}
	while (Step()) {}
	return 0;
}

void EngineApplication::RequestStop() noexcept
{
	stop_requested_.store(true, std::memory_order_relaxed);
}

bool EngineApplication::IsStopRequested() const noexcept
{
	return stop_requested_.load(std::memory_order_relaxed);
}

bool EngineApplication::IsRunning() const noexcept
{
	return runtime_ != nullptr;
}
