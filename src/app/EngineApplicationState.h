#pragma once

#include "EngineConfiguration.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ve::engine
{
	class EngineRuntime;

	/** Storage shared by the application facade and its split implementation units. */
	class EngineApplicationState
	{
	protected:
		EngineApplicationState() = default;
		~EngineApplicationState();

		std::optional<ValidatedEngineCreateInfo> create_info_;
		std::vector<std::string> create_info_validation_issues_;
		std::unique_ptr<EngineRuntime> runtime_;
	};
}
