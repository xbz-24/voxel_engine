#include <doctest/doctest.h>

#include "Block.h"
#include "World.h"
#include "WorldBlockEdit.h"

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
