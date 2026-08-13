#include "EngineInstanceImpl.h"

namespace voxel
{
	int Engine::Impl::Run()
	{
		if (!validation_issues_.empty())
		{
			LogValidationIssues();
			return -1;
		}
		return runtime_ ? runtime_->Run() : -1;
	}

	bool Engine::Impl::Start()
	{
		return StartDetailed().failure == EngineStartFailure::None;
	}

	EngineStartResult Engine::Impl::StartDetailed()
	{
		if (!validation_issues_.empty())
		{
			LogValidationIssues();
			return EngineStartResult::InvalidConfiguration(validation_issues_);
		}
		if (runtime_ == nullptr)
		{
			return EngineStartResult::RuntimeUnavailable("Engine runtime was not created");
		}
		const ve::engine::EngineStartupResult startup_result = runtime_->Start();
		if (startup_result)
		{
			return EngineStartResult::Success();
		}
		return EngineStartResult::RuntimeStartupFailed(startup_result.message);
	}

	bool Engine::Impl::Step()
	{
		if (!validation_issues_.empty() || runtime_ == nullptr)
		{
			return false;
		}
		return runtime_->Step();
	}

	void Engine::Impl::Shutdown() noexcept
	{
		if (runtime_ != nullptr)
		{
			runtime_->Shutdown();
		}
	}

	void Engine::Impl::RequestStop() noexcept
	{
		if (runtime_ != nullptr)
		{
			runtime_->RequestStop();
		}
	}

	bool Engine::Impl::IsRunning() const noexcept
	{
		return runtime_ != nullptr && runtime_->IsRunning();
	}

	void Engine::Impl::LogValidationIssues() const
	{
		if (!on_log_) return;
		for (const std::string& issue : validation_issues_)
		{
			on_log_("Invalid EngineConfig: " + issue);
		}
	}
}
