#pragma once

#include "EngineConfiguration.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ve::engine
{
	class EngineRuntime;
	enum class EngineApplicationPhase
	{
		Idle,
		Starting,
		Running,
		Stepping,
		Stopping
	};

	/** Storage shared by the application facade and its split implementation units. */
	class EngineApplicationState
	{
	protected:
		EngineApplicationState() = default;
		~EngineApplicationState();

		std::optional<ValidatedEngineCreateInfo> create_info_;
		std::vector<std::string> create_info_validation_issues_;
		std::unique_ptr<EngineRuntime> runtime_;
		EngineApplicationPhase phase_ = EngineApplicationPhase::Idle;
		bool shutdown_requested_ = false;
	};
}
