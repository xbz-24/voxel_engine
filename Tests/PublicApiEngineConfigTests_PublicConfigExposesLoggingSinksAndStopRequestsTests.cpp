#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public config exposes logging sinks and stop requests")
{
	CHECK_FALSE(voxel::LogSettings{}.file_output_enabled);
	const voxel::LogSettings logging = voxel::LogSettings{}
		.WithMinimumLevel(voxel::LogLevel::Warning)
		.DisableConsole()
		.WriteToFile("custom-engine.log");

	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithLogging(logging)
		.WithLogLevel(voxel::LogLevel::Error)
		.EnableConsoleLogging()
		.DisableFileLogging();

	voxel::Engine engine{ config };
	engine.RequestStop();

	CHECK(config.logging.minimum_level == voxel::LogLevel::Error);
	CHECK(config.logging.console_enabled);
	CHECK(!config.logging.file_output_enabled);
	CHECK(config.logging.file_output_path.empty());
}

TEST_CASE("public config rejects enabled file logging without a destination")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithLogging(voxel::LogSettings{}.WriteToFile(""));
	const std::vector<std::string> issues = config.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"logging.file_output_path must not be empty when file output is enabled") !=
		issues.end());
}

