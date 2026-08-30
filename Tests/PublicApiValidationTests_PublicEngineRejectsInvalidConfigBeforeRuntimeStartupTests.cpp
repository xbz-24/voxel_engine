#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public engine rejects invalid config before runtime startup")
{
	std::vector<std::string> logs;
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow("Invalid", 0, 720)
		.OnLog([&logs](const std::string& line) {
			logs.push_back(line);
		});

	voxel::Engine engine{ invalid };

	CHECK(engine.Run() == -1);
	REQUIRE(!logs.empty());
	CHECK(logs.front() == "Invalid EngineConfig: window.width must be greater than zero");
}

