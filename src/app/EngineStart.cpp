#include "Engine.h"

#include "EngineRuntime.h"
#include "VoxelSandboxModule.h"
#include "VoxelSandboxRuntimeConfiguration.h"

#include <utility>

bool EngineApplication::Start()
{
	return static_cast<bool>(StartDetailed());
}

ve::engine::EngineStartupResult EngineApplication::StartDetailed()
{
	using ve::engine::EngineApplicationPhase;
	using ve::engine::EngineStartupFailure;
	using ve::engine::EngineStartupResult;
	if (phase_ == EngineApplicationPhase::Running) return EngineStartupResult::Success();
	if (phase_ != EngineApplicationPhase::Idle)
	{
		return EngineStartupResult::Failure(EngineStartupFailure::RuntimeOperationInProgress,
			"Engine lifecycle operation is already in progress");
	}
	if (!HasValidCreateInfo())
	{
		const std::string message = create_info_validation_issues_.empty()
			? "EngineCreateInfo is invalid"
			: "EngineCreateInfo is invalid: " + create_info_validation_issues_.front();
		return EngineStartupResult::Failure(EngineStartupFailure::InvalidCreateInfo, message);
	}

	phase_ = EngineApplicationPhase::Starting;
	shutdown_requested_ = false;
	std::unique_ptr<ve::engine::EngineRuntime> runtime;
	try
	{
		auto module = std::make_unique<ve::engine::VoxelSandboxModule>(*this);
		runtime = std::make_unique<ve::engine::EngineRuntime>(
			ve::engine::CreateRuntimeHostConfiguration(CreateInfo()), std::move(module));
		const EngineStartupResult result = runtime->Start();
		if (!result)
		{
			StopRuntime(std::move(runtime));
			return result;
		}
		if (shutdown_requested_)
		{
			StopRuntime(std::move(runtime));
			return EngineStartupResult::Failure(EngineStartupFailure::RuntimeStartupCancelled,
				"Engine startup was cancelled by Shutdown()");
		}
		runtime_ = std::move(runtime);
		phase_ = EngineApplicationPhase::Running;
		return EngineStartupResult::Success();
	}
	catch (...)
	{
		StopRuntime(std::move(runtime));
		throw;
	}
}
