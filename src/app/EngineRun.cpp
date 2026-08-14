#include "Engine.h"

#include "EngineRuntime.h"
#include "VoxelSandboxModule.h"
#include "VoxelSandboxRuntimeConfiguration.h"

bool EngineApplication::Start()
{
	return StartDetailed().failure == ve::engine::EngineStartupFailure::None;
}

ve::engine::EngineStartupResult EngineApplication::StartDetailed()
{
	if (!HasValidCreateInfo())
	{
		const std::string message = create_info_validation_issues_.empty()
			? "EngineCreateInfo is invalid"
			: "EngineCreateInfo is invalid: " + create_info_validation_issues_.front();
		return ve::engine::EngineStartupResult::Failure(
			ve::engine::EngineStartupFailure::InvalidCreateInfo,
			message);
	}
	if (runtime_ != nullptr)
	{
		return ve::engine::EngineStartupResult::Success();
	}
	auto module = std::make_unique<ve::engine::VoxelSandboxModule>(*this);
	auto runtime = std::make_unique<ve::engine::EngineRuntime>(
		ve::engine::CreateRuntimeHostConfiguration(CreateInfo()),
		std::move(module));
	const ve::engine::EngineStartupResult startup_result = runtime->Start();
	if (!startup_result)
	{
		return startup_result;
	}
	runtime_ = std::move(runtime);
	return ve::engine::EngineStartupResult::Success();
}

bool EngineApplication::Step()
{
	if (runtime_ == nullptr)
	{
		return false;
	}
	try
	{
		const bool should_request_next_frame = runtime_->Step();
		if (!should_request_next_frame) Shutdown();
		return should_request_next_frame;
	}
	catch (...)
	{
		Shutdown();
		throw;
	}
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
	if (runtime_ != nullptr) runtime_->RequestStop();
}

bool EngineApplication::IsRunning() const noexcept
{
	return runtime_ != nullptr;
}
