#pragma once

#include "EngineRuntimeBridge.h"

#include <memory>
#include <string>
#include <vector>

namespace voxel
{
	class Engine::Impl
	{
	public:
		explicit Impl(const EngineConfig& config);

		[[nodiscard]] int Run();
		[[nodiscard]] bool Start();
		[[nodiscard]] EngineStartResult StartDetailed();
		[[nodiscard]] bool Step();
		void Shutdown() noexcept;
		void RequestStop() noexcept;
		[[nodiscard]] bool IsRunning() const noexcept;

	private:
		explicit Impl(const EngineConfig& config, detail::EngineServices services);
		void LogValidationIssues() noexcept;

		std::vector<std::string> validation_issues_;
		LogCallback on_log_;
		std::unique_ptr<detail::IEngineRuntime> runtime_;
		bool reporting_validation_issues_ = false;
	};
}
