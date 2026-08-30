#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <string>
#include <vector>

TEST_CASE("public engine config accepts the maximum fixed world size")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithWorldSizeChunks(voxel::MaximumWorldSizeChunks);

	CHECK(config.world.size_chunks == voxel::MaximumWorldSizeChunks);
	CHECK(config.Validate().empty());
	CHECK(config.IsValid());
}

TEST_CASE("public engine rejects an oversized world before runtime startup")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithWorldSizeChunks(voxel::MaximumWorldSizeChunks + 1);
	const std::vector<std::string> issues = config.Validate();

	CHECK_FALSE(config.IsValid());
	REQUIRE(issues.size() == 1U);
	CHECK(issues.front() == "world.size_chunks must be no greater than 64");

	voxel::Engine engine{ config };
	const voxel::EngineStartResult result = engine.StartDetailed();

	CHECK(!result);
	CHECK(result.failure == voxel::EngineStartFailure::InvalidConfiguration);
	CHECK(result.issues == issues);
	CHECK(result.message == "EngineConfig is invalid");
	CHECK_FALSE(engine.IsRunning());
}
