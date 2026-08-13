#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public config exposes logging sinks and stop requests")
{
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

