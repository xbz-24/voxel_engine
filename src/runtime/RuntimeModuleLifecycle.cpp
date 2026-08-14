#include "RuntimeModuleLifecycle.h"

#include <utility>

namespace ve::engine
{
	RuntimeModuleLifecycle::RuntimeModuleLifecycle(
		std::unique_ptr<IRuntimeModule> module) noexcept
		: module_(std::move(module))
	{
	}

	RuntimeModuleLifecycle::~RuntimeModuleLifecycle() noexcept
	{
		Shutdown();
	}

	EngineStartupResult RuntimeModuleLifecycle::Initialize(RuntimeModuleContext& context)
	{
		if (!module_)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::RuntimeModuleUnavailable,
				"Engine runtime content module was not created");
		}
		if (initialized_) return EngineStartupResult::Success();
		if (shutdown_pending_)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::RuntimeModuleUnavailable,
				"Runtime module must be shut down before another initialization");
		}
		shutdown_pending_ = true;
		try
		{
			const EngineStartupResult result = module_->Initialize(context);
			initialized_ = static_cast<bool>(result);
			return result;
		}
		catch (...)
		{
			Shutdown();
			throw;
		}
	}

	void RuntimeModuleLifecycle::RunFrame(RuntimeModuleContext& context)
	{
		if (initialized_) module_->RunFrame(context);
	}

	void RuntimeModuleLifecycle::Shutdown() noexcept
	{
		initialized_ = false;
		const bool shutdown_pending = std::exchange(shutdown_pending_, false);
		std::unique_ptr<IRuntimeModule> module = std::move(module_);
		if (shutdown_pending && module) module->Shutdown();
	}

	bool RuntimeModuleLifecycle::HasModule() const noexcept
	{
		return module_ != nullptr;
	}

	bool RuntimeModuleLifecycle::IsActive() const noexcept
	{
		return initialized_;
	}
}
