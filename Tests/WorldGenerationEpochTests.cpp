#include <doctest/doctest.h>

#include "World.h"

#include <algorithm>
#include <cstdint>

namespace
{
	bool ChunkContainsOnlyAir(const Chunk& chunk)
	{
		return std::ranges::all_of(chunk.CreateMeshInput().blocks,
			[](ve::blocks::BlockId block) { return block == ve::blocks::BlockId::Air; });
	}
}

TEST_CASE("world rejects generated terrain captured before a same-size respawn")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	ve::world::generation::ChunkGenerationResult result_a{};
	result_a.blocks.fill(ve::blocks::BlockId::Stone);
	result_a.chunkStorageRevision = world.ChunkStorageRevision();

	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	REQUIRE(world.Chunks().size() == 1);
	const std::uint64_t revision_b = world.Revision();
	const std::uint64_t storage_revision_b = world.ChunkStorageRevision();
	const std::uint64_t mesh_revision_b = world.Chunks().front().MeshRevision();
	REQUIRE(result_a.chunkStorageRevision != storage_revision_b);
	REQUIRE(ChunkContainsOnlyAir(world.Chunks().front()));
	REQUIRE_FALSE(world.Chunks().front().IsGenerated());
	REQUIRE(world.DirtyChunks().empty());
	REQUIRE(world.PendingEventCount() == 0);

	CHECK_FALSE(world.ApplyGeneratedChunk(result_a));
	CHECK(ChunkContainsOnlyAir(world.Chunks().front()));
	CHECK_FALSE(world.Chunks().front().IsGenerated());
	CHECK(world.Chunks().front().MeshRevision() == mesh_revision_b);
	CHECK(world.Revision() == revision_b);
	CHECK(world.ChunkStorageRevision() == storage_revision_b);
	CHECK(world.DirtyChunks().empty());
	CHECK(world.DrainEvents().empty());

	result_a.chunkStorageRevision = storage_revision_b;
	CHECK(world.ApplyGeneratedChunk(result_a));
}
