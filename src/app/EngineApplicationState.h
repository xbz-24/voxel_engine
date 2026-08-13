#pragma once

#include "EngineConfiguration.h"
#include "EngineState.h"
#include "RuntimeSettings.h"

#include <atomic>
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

		EngineRenderCacheState _render_cache_state;
		EngineWindowState _window_state;
		ve::gameplay::RuntimeSettings runtime_settings_;
		std::optional<ValidatedEngineCreateInfo> create_info_;
		std::vector<std::string> create_info_validation_issues_;
		std::atomic_bool stop_requested_{ false };
		std::unique_ptr<EngineRuntime> runtime_;
	};
}
