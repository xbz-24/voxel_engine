#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public engine exposes nonblocking frame lifecycle")
{
	const voxel::EngineConfig invalid_config = voxel::EngineConfig::Default()
		.WithWindow(voxel::Window("Invalid", 0, 600));

	voxel::Engine engine{ invalid_config };

	CHECK(!engine.IsRunning());
	const voxel::EngineStartResult start_result = engine.StartDetailed();
	CHECK(!start_result);
	CHECK(start_result.failure == voxel::EngineStartFailure::InvalidConfiguration);
	CHECK(start_result.message == "EngineConfig is invalid");
	REQUIRE(start_result.issues.size() == 1);
	CHECK(start_result.issues.front() == "window.width must be greater than zero");
	CHECK(!engine.Start());
	CHECK(!engine.Step());
	engine.Shutdown();
	CHECK(!engine.IsRunning());
}

