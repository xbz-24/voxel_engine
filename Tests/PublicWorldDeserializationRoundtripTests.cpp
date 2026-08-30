#include "WorldLoadTestSupport.h"

#include <doctest/doctest.h>

using namespace world_load_test;

TEST_CASE("strict world loading exactly roundtrips every public world field")
{
	const TemporaryWorldPath path;
	const voxel::WorldConfig expected = SentinelWorld();
	voxel::WorldConfig output{};

	REQUIRE(voxel::SaveWorldConfig(expected, path.Get().string()));
	REQUIRE(voxel::TryLoadWorldConfig(path.Get().string(), output));
	CHECK(SameWorld(output, expected));
}
