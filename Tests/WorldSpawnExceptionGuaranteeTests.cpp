#include "WorldSpawnExceptionTestSupport.h"

#include <doctest/doctest.h>

#include <new>
#include <stdexcept>

TEST_CASE("empty world respawn preserves state when mesh creation throws")
{
	ve::tests::ThrowingMeshCreationBackend backend{ 2 };
	ve::world::World world(ve::world::CreateInfoForSquareWorld(2));
	ve::tests::SeedWorldForRespawnFailure(world);
	const ve::tests::WorldSnapshot before = ve::tests::CaptureWorldSnapshot(world);
	world.SetRenderBackend(&backend);

	CHECK_THROWS_AS(
		world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 2 }),
		std::runtime_error);
	CHECK(backend.CallCount() == 2);
	CHECK(ve::tests::CaptureWorldSnapshot(world) == before);
}

TEST_CASE("flat world respawn preserves state when fixed reserve is exhausted")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	ve::tests::SeedWorldForRespawnFailure(world);
	const ve::tests::WorldSnapshot before = ve::tests::CaptureWorldSnapshot(world);

	CHECK_THROWS_AS(
		world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 2 }),
		std::bad_alloc);
	CHECK(ve::tests::CaptureWorldSnapshot(world) == before);
}
