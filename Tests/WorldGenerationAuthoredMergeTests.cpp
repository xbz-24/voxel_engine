#include <doctest/doctest.h>

#include "World.h"

#include <cstddef>
#include <cstdint>
#include <utility>

namespace
{
	ve::world::generation::ChunkGenerationResult DirtResult(const ve::world::World& world)
	{
		ve::world::generation::ChunkGenerationResult result{};
		result.blocks.fill(ve::blocks::BlockId::Dirt);
		result.chunkStorageRevision = world.ChunkStorageRevision();
		return result;
	}

	void CheckCombinedProvenance(const Chunk& chunk)
	{
		CHECK(chunk.IsGenerated());
		CHECK(chunk.HasProceduralTerrain());
		CHECK(chunk.HasAuthoredEdits());
		CHECK(chunk.Provenance() == ChunkContentProvenance::ProceduralTerrainWithAuthoredEdits);
	}
}

TEST_CASE("generated terrain merges authored blocks and rejects a duplicate result")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	const auto result = DirtResult(world);
	REQUIRE(world.SetBlock(1, 64, 1, ve::blocks::BlockId::DiamondOre));
	const std::uint64_t revision_before_merge = world.Revision();
	const std::size_t events_before_merge = world.PendingEventCount();

	REQUIRE(world.ApplyGeneratedChunk(result));
	CHECK(world.GetBlock(1, 64, 1) == ve::blocks::BlockId::DiamondOre);
	CHECK(world.GetBlock(2, 64, 1) == ve::blocks::BlockId::Dirt);
	CheckCombinedProvenance(world.Chunks().front());
	CHECK(world.Revision() > revision_before_merge);
	CHECK(world.PendingEventCount() == events_before_merge + 1);
	const std::uint64_t revision_after_merge = world.Revision();
	const std::uint64_t mesh_revision_after_merge = world.Chunks().front().MeshRevision();
	const std::size_t events_after_merge = world.PendingEventCount();
	const std::size_t dirty_chunks_after_merge = world.DirtyChunks().size();

	CHECK_FALSE(world.ApplyGeneratedChunk(result));
	CHECK(world.GetBlock(1, 64, 1) == ve::blocks::BlockId::DiamondOre);
	CHECK(world.GetBlock(2, 64, 1) == ve::blocks::BlockId::Dirt);
	CheckCombinedProvenance(world.Chunks().front());
	CHECK(world.Revision() == revision_after_merge);
	CHECK(world.Chunks().front().MeshRevision() == mesh_revision_after_merge);
	CHECK(world.PendingEventCount() == events_after_merge);
	CHECK(world.DirtyChunks().size() == dirty_chunks_after_merge);
}

TEST_CASE("generated terrain preserves an explicit air write over placeholder air")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	const auto result = DirtResult(world);
	REQUIRE(world.SetBlock(1, 64, 1, ve::blocks::BlockId::Air));
	REQUIRE(world.GetBlock(1, 64, 1) == ve::blocks::BlockId::Air);
	REQUIRE(world.Chunks().front().HasAuthoredEdits());
	const std::uint64_t revision_before_merge = world.Revision();
	const std::size_t events_before_merge = world.PendingEventCount();

	REQUIRE(world.ApplyGeneratedChunk(result));
	CHECK(world.GetBlock(1, 64, 1) == ve::blocks::BlockId::Air);
	CHECK(world.GetBlock(2, 64, 1) == ve::blocks::BlockId::Dirt);
	CheckCombinedProvenance(world.Chunks().front());
	CHECK(world.Revision() > revision_before_merge);
	CHECK(world.PendingEventCount() == events_before_merge + 1);
}

TEST_CASE("moving a pending chunk preserves its authored block overlay")
{
	Chunk source(0, 0, ChunkGenerationMode::Empty);
	REQUIRE(source.SetBlock(1, 64, 1, ve::blocks::BlockId::DiamondOre));
	Chunk constructed(std::move(source));
	Chunk moved(1, 1, ChunkGenerationMode::Empty);
	moved = std::move(constructed);
	ve::world::generation::ChunkGenerationResult result{};
	result.blocks.fill(ve::blocks::BlockId::Dirt);

	REQUIRE(moved.ReplaceBlocks(result.blocks));
	CHECK(moved.GetBlock(1, 64, 1) == ve::blocks::BlockId::DiamondOre);
	CHECK(moved.GetBlock(2, 64, 1) == ve::blocks::BlockId::Dirt);
	CheckCombinedProvenance(moved);
}
