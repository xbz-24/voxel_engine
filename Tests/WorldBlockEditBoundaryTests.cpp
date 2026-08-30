#include <doctest/doctest.h>

#include "Block.h"
#include "ChunkTerrain.h"
#include "World.h"
#include "WorldBlockEdit.h"

#include <cstddef>
#include <limits>

namespace
{
	[[nodiscard]] bool ApplyPointBox(ve::world::World& world, int x, int y, int z)
	{
		return ve::world::ApplyWorldBlockEdit(
			world,
			ve::world::MakeBlockBoxEdit(x, y, z, x, y, z, ve::blocks::BlockId::Stone));
	}
}

TEST_CASE("world block box edits terminate at the maximum integer coordinate")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });

	constexpr int maximum = std::numeric_limits<int>::max();
	CHECK_FALSE(ApplyPointBox(world, maximum, 0, 0));
	CHECK_FALSE(ApplyPointBox(world, 0, maximum, 0));
	CHECK_FALSE(ApplyPointBox(world, 0, 0, maximum));
}

TEST_CASE("world block box edits clip extreme ranges to loaded chunks")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	constexpr int minimum = std::numeric_limits<int>::lowest();
	constexpr int maximum = std::numeric_limits<int>::max();

	CHECK_FALSE(ve::world::ApplyWorldBlockEdit(world, ve::world::MakeBlockBoxEdit(
		minimum, minimum, minimum, maximum, maximum, maximum,
		ve::blocks::BlockId::Stone)));
	CHECK(world.PendingEventCount() ==
		static_cast<std::size_t>(ve::world::terrain::ChunkBlockCount));
	CHECK(world.GetBlock(0, 0, 0) == ve::blocks::BlockId::Stone);
	CHECK(world.GetBlock(15, 127, 15) == ve::blocks::BlockId::Stone);
}

TEST_CASE("world block box edits report partial loaded intersections")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });

	CHECK_FALSE(ve::world::ApplyWorldBlockEdit(world, ve::world::MakeBlockBoxEdit(
		-1, 0, 0, 0, 0, 0, ve::blocks::BlockId::Stone)));
	CHECK(world.PendingEventCount() == 1U);
	CHECK(world.GetBlock(0, 0, 0) == ve::blocks::BlockId::Stone);
}

TEST_CASE("world block box edits accept an exact loaded chunk")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });

	CHECK(ve::world::ApplyWorldBlockEdit(world, ve::world::MakeBlockBoxEdit(
		0, 0, 0, 15, 127, 15, ve::blocks::BlockId::Stone)));
	CHECK(world.PendingEventCount() ==
		static_cast<std::size_t>(ve::world::terrain::ChunkBlockCount));
}

TEST_CASE("world block box edits accumulate coverage across loaded chunks")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(2));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 2 });

	CHECK(ve::world::ApplyWorldBlockEdit(world, ve::world::MakeBlockBoxEdit(
		15, 0, 0, 16, 0, 0, ve::blocks::BlockId::Stone)));
	CHECK(world.PendingEventCount() == 2U);
	CHECK(world.GetBlock(15, 0, 0) == ve::blocks::BlockId::Stone);
	CHECK(world.GetBlock(16, 0, 0) == ve::blocks::BlockId::Stone);
}
