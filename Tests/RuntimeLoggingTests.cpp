#include <doctest/doctest.h>

#include "EngineRuntime.h"
#include "Logger.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <system_error>
#include <utility>

namespace
{
	class InactiveRuntimeModule final : public ve::engine::IRuntimeModule
	{
	public:
		ve::engine::EngineStartupResult Initialize(ve::engine::RuntimeModuleContext&) override
		{ return ve::engine::EngineStartupResult::Success(); }
		void RunFrame(ve::engine::RuntimeModuleContext&) override {}
		void Shutdown() noexcept override {}
	};

	std::filesystem::path MakeBlockingLogParent()
	{
		const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
		const std::filesystem::path root = std::filesystem::temp_directory_path() /
			("voxel_runtime_logging_" + std::to_string(unique));
		std::filesystem::create_directories(root);
		const std::filesystem::path blocking_file = root / "not-a-directory";
		std::ofstream{ blocking_file } << "blocks child creation";
		return blocking_file;
	}
}

TEST_CASE("runtime reports file logging failure and releases global state")
{
	const std::filesystem::path blocking_file = MakeBlockingLogParent();
	ve::engine::RuntimeHostConfiguration configuration{};
	configuration.logging.console_enabled = false;
	configuration.logging.file_output_enabled = true;
	configuration.logging.file_output_path = blocking_file / "engine.log";
	int callback_count = 0;
	configuration.on_log = [&callback_count](const std::string&) { ++callback_count; };
	auto module = std::make_unique<InactiveRuntimeModule>();
	ve::engine::EngineRuntime runtime(std::move(configuration), std::move(module));

	const ve::engine::EngineStartupResult result = runtime.Start();
	CHECK_FALSE(result);
	CHECK(result.failure == ve::engine::EngineStartupFailure::LoggingInitializationFailed);
	CHECK(result.message == "The configured log file could not be opened");
	CHECK(callback_count == 1);
	ve::log::Info("callback is detached after failed runtime startup");
	CHECK(callback_count == 1);
	ve::engine::RuntimeHostConfiguration next_configuration{};
	next_configuration.logging.console_enabled = false;
	next_configuration.logging.file_output_enabled = true;
	next_configuration.logging.file_output_path = blocking_file / "engine.log";
	auto next_module = std::make_unique<InactiveRuntimeModule>();
	ve::engine::EngineRuntime next_runtime(
		std::move(next_configuration), std::move(next_module));
	const ve::engine::EngineStartupResult next_result = next_runtime.Start();
	CHECK(next_result.failure == ve::engine::EngineStartupFailure::LoggingInitializationFailed);
	CHECK(next_result.message == "The configured log file could not be opened");

	std::error_code error;
	std::filesystem::remove_all(blocking_file.parent_path(), error);
}
