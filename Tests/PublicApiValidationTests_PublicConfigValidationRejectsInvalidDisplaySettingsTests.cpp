#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public config validation rejects invalid display settings")
{
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow(voxel::Window("Invalid display")
			.UseMonitor(-1)
			.WithRefreshRate(-60));

	const std::vector<std::string> issues = invalid.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"window.monitor_index must be zero or greater") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"window.refresh_rate_hertz must be zero or greater") != issues.end());
}

