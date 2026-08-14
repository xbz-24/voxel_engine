#include "EngineRuntime.h"

#include "Logger.h"

#include <filesystem>
#include <optional>

namespace ve::engine
{
	namespace
	{
		std::atomic_bool runtime_logging_session_active{ false };
	}

	bool EngineRuntime::TryAcquireLoggingSession() noexcept
	{
		if (owns_logging_session_) return true;
		bool expected = false;
		owns_logging_session_ = runtime_logging_session_active.compare_exchange_strong(
			expected, true, std::memory_order_acq_rel);
		return owns_logging_session_;
	}

	EngineStartupResult EngineRuntime::PrepareAssetsAndLogging()
	{
		if (!TryAcquireLoggingSession())
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::LoggingInitializationFailed,
				"Another engine runtime already owns process-wide logging");
		}
		ve::assets::AssetPathResolveOptions options;
		options.explicit_asset_directory = configuration_.asset_directory;
		options.explicit_vulkan_shader_directory = configuration_.vulkan_shader_directory;
		options.search_roots = configuration_.asset_search_roots;
		asset_paths_ = ve::assets::Resolve(options);
		ve::log::SetCallback(configuration_.on_log);
		if (configuration_.logging.file_output_enabled &&
			configuration_.logging.file_output_path.empty())
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::LoggingInitializationFailed,
				"File logging is enabled but no output path was configured");
		}
		std::optional<std::filesystem::path> file_output_path;
		if (configuration_.logging.file_output_enabled &&
			!configuration_.logging.file_output_path.empty())
		{
			file_output_path = configuration_.logging.file_output_path;
		}
		const bool logging_ready = ve::log::ApplyConfiguration(ve::log::LoggerConfiguration{
			configuration_.logging.minimum_level,
			configuration_.logging.console_enabled,
			file_output_path
		});
		if (!logging_ready)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::LoggingInitializationFailed,
				"The configured log file could not be opened");
		}
		VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime started");
		return EngineStartupResult::Success();
	}

	void EngineRuntime::ReleaseLoggingSession() noexcept
	{
		if (!owns_logging_session_) return;
		try { ve::log::ResetRuntimeState(); }
		catch (...) {}
		owns_logging_session_ = false;
		runtime_logging_session_active.store(false, std::memory_order_release);
	}
}
