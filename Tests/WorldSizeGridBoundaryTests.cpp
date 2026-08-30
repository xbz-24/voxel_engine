#include <doctest/doctest.h>

#include "WorldConfiguration.h"
#include "WorldGridMath.h"

static_assert(ve::world::MaximumWorldSizeChunks == 64);
static_assert(ve::world::grid::SquareChunkCapacity(
	ve::world::MaximumWorldSizeChunks) == 4096U);
static_assert(ve::world::grid::FlattenWorldChunkIndex(
	ve::world::MaximumWorldSizeChunks - 1,
	ve::world::MaximumWorldSizeChunks - 1,
	ve::world::MaximumWorldSizeChunks) == 4095U);

TEST_CASE("maximum fixed world size maps to bounded chunk storage indices")
{
	CHECK(ve::world::ChunkCapacityForSquareWorld(
		ve::world::MaximumWorldSizeChunks) == 4096U);
	CHECK(ve::world::grid::FlattenWorldChunkIndex(
		ve::world::MaximumWorldSizeChunks - 1,
		ve::world::MaximumWorldSizeChunks - 1,
		ve::world::MaximumWorldSizeChunks) == 4095U);
}
