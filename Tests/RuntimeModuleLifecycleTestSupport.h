#pragma once

#include "RuntimeModule.h"

#include <memory>
#include <stdexcept>
#include <utility>

namespace ve::tests
{
	enum class ModuleInitializeBehavior
	{
		Succeed,
		Fail,
		Throw
	};

	struct RuntimeModuleLifecycleState
	{
		int initialize_count = 0;
		int shutdown_count = 0;
		int destructor_count = 0;
		bool shutdown_preceded_destruction = false;
	};

	class LifecycleTestModule final : public ve::engine::IRuntimeModule
	{
	public:
		LifecycleTestModule(std::shared_ptr<RuntimeModuleLifecycleState> state,
			ModuleInitializeBehavior behavior) noexcept
			: state_(std::move(state)), behavior_(behavior) {}

		~LifecycleTestModule() override
		{
			++state_->destructor_count;
			state_->shutdown_preceded_destruction = state_->shutdown_count == 1;
		}

		ve::engine::EngineStartupResult Initialize(
			ve::engine::RuntimeModuleContext&) override
		{
			++state_->initialize_count;
			if (behavior_ == ModuleInitializeBehavior::Throw)
				throw std::runtime_error("module initialization failed");
			if (behavior_ == ModuleInitializeBehavior::Fail)
			{
				return ve::engine::EngineStartupResult::Failure(
					ve::engine::EngineStartupFailure::RuntimeModuleUnavailable,
					"module returned failure");
			}
			return ve::engine::EngineStartupResult::Success();
		}

		void RunFrame(ve::engine::RuntimeModuleContext&) override {}
		void Shutdown() noexcept override { ++state_->shutdown_count; }

	private:
		std::shared_ptr<RuntimeModuleLifecycleState> state_;
		ModuleInitializeBehavior behavior_;
	};
}
