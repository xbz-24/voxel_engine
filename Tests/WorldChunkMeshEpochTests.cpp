#include <doctest/doctest.h>

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "ChunkMeshRequest.h"
#include "World.h"

#include <cstdint>
#include <utility>

TEST_CASE("world rejects mesh output captured before a same-revision respawn")
{
	const ve::blocks::BlockRegistry block_registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });

	const auto request_a = world.CaptureChunkMeshBuildRequest(0, 0);
	REQUIRE(request_a.has_value());
	auto output_a = ve::world::mesh::BuildChunkMeshOutput(*request_a, block_registry);
	CHECK(output_a.chunkStorageRevision == request_a->chunkStorageRevision);
	const std::uint64_t mesh_revision_a = request_a->chunk.revision;

	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	REQUIRE(world.Chunks().size() == 1);
	REQUIRE(world.Chunks().front().MeshRevision() == mesh_revision_a);
	REQUIRE(world.Chunks().front().NeedsMeshBuild());
	CHECK_FALSE(world.TryUploadChunkMeshOutput(std::move(output_a)));
	CHECK(world.Chunks().front().NeedsMeshBuild());

	const auto request_b = world.CaptureChunkMeshBuildRequest(0, 0);
	REQUIRE(request_b.has_value());
	CHECK(request_b->chunkStorageRevision != request_a->chunkStorageRevision);
	auto output_b = ve::world::mesh::BuildChunkMeshOutput(*request_b, block_registry);
	CHECK(output_b.chunkStorageRevision == request_b->chunkStorageRevision);
	CHECK(world.TryUploadChunkMeshOutput(std::move(output_b)));
	CHECK_FALSE(world.Chunks().front().NeedsMeshBuild());
}

TEST_CASE("stale failed mesh output preserves the new chunk build reservation")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	const auto request_a = world.CaptureChunkMeshBuildRequest(0, 0);
	REQUIRE(request_a.has_value());

	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	REQUIRE(world.Chunks().size() == 1);
	Chunk& chunk_b = const_cast<Chunk&>(world.Chunks().front());
	REQUIRE(chunk_b.MeshRevision() == request_a->chunk.revision);
	REQUIRE(request_a->chunkStorageRevision != world.ChunkStorageRevision());
	REQUIRE(chunk_b.TryReserveMeshBuild());
	ve::world::mesh::ChunkMeshBuildOutput stale_failure{
		0,
		0,
		chunk_b.MeshRevision(),
		{},
		false,
		request_a->chunkStorageRevision
	};

	CHECK_FALSE(world.TryUploadChunkMeshOutput(std::move(stale_failure)));
	CHECK(chunk_b.HasPendingMeshBuildReservation());
	CHECK(chunk_b.NeedsMeshBuild());
}
