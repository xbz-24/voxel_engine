#include <doctest/doctest.h>

#include "Chunk.h"
#include "ChunkMeshFailureRecovery.h"
#include "ChunkMeshUploader.h"

TEST_CASE("failed chunk mesh output releases the matching build reservation")
{
	Chunk chunk(2, 3, ChunkGenerationMode::Empty);
	REQUIRE(chunk.TryReserveMeshBuild());
	const ve::world::mesh::ChunkMeshBuildOutput failedOutput{
		2,
		3,
		chunk.MeshRevision(),
		{},
		false
	};

	CHECK_FALSE(ve::world::mesh::TryUploadChunkMeshOutput(chunk, failedOutput));
	CHECK_FALSE(chunk.HasPendingMeshBuildReservation());
	CHECK(chunk.NeedsMeshBuild());
	CHECK(chunk.TryReserveMeshBuild());
}

TEST_CASE("stale chunk mesh failure preserves a newer build reservation")
{
	Chunk chunk(2, 3, ChunkGenerationMode::Empty);
	REQUIRE(chunk.TryReserveMeshBuild());
	const ve::world::mesh::ChunkMeshBuildOutput staleFailure{
		2,
		3,
		chunk.MeshRevision(),
		{},
		false
	};
	chunk.MarkDirty();
	REQUIRE(chunk.TryReserveMeshBuild());

	CHECK_FALSE(ve::world::mesh::TryUploadChunkMeshOutput(chunk, staleFailure));
	CHECK(chunk.HasPendingMeshBuildReservation());
}

TEST_CASE("fail-safe chunk mesh recovery releases every pending reservation")
{
	Chunk chunk(2, 3, ChunkGenerationMode::Empty);
	REQUIRE(chunk.TryReserveMeshBuild());
	ve::world::mesh::CancelPendingChunkMeshReservations({ &chunk, 1 });

	CHECK_FALSE(chunk.HasPendingMeshBuildReservation());
	CHECK(chunk.TryReserveMeshBuild());
}
