#include "Chunk.h"

#include "ChunkMeshBuilder.h"

#include <span>
#include <utility>

using ve::blocks::BlockRegistry;

/// Builds a CPU mesh and uploads it immediately on the calling thread.
void Chunk::BuildMesh(const BlockRegistry& block_registry, const ve::world::mesh::NeighborChunks& neighbors)
{
	UploadMesh(ve::world::mesh::BuildChunkMesh(*this, block_registry, neighbors));
}

/// Uploads finished CPU mesh data to this chunk's GPU buffer.
void Chunk::UploadMesh(ve::world::mesh::ChunkMeshBuildResult mesh_build_result)
{
	mesh_.Upload(mesh_build_result.vertices, std::move(mesh_build_result.batches));
	is_mesh_built_ = true;
	is_mesh_build_queued_ = false;
}

/// Returns the uploaded render mesh resource.
const ve::rendering::ChunkGpuMesh& Chunk::RenderMesh() const noexcept
{
	return mesh_;
}

/// Returns a read-only mesh input view over this chunk's block storage.
ve::world::mesh::ChunkMeshInput Chunk::CreateMeshInput() const noexcept
{
	const auto* first_block = &blocks_[0][0][0];
	return ve::world::mesh::ChunkMeshInput{
		chunk_x_,
		chunk_z_,
		std::span<const ve::blocks::BlockId>(first_block, ve::world::mesh::ChunkBlockCount)
	};
}

/// Reports whether this chunk needs a fresh mesh.
bool Chunk::NeedsMeshBuild() const noexcept
{
	return !is_mesh_built_;
}

/**
 * Reserves this chunk for one async mesh task.
 *
 * @return True when the caller should enqueue a mesh build.
 */
bool Chunk::TryReserveMeshBuild() noexcept
{
	if (!NeedsMeshBuild() || is_mesh_build_queued_) return false;
	is_mesh_build_queued_ = true;
	return true;
}

bool Chunk::HasPendingMeshBuildReservation() const noexcept
{
	return is_mesh_build_queued_;
}

/// Releases a failed async mesh reservation without marking the current mesh valid.
void Chunk::CancelMeshBuildReservation() noexcept
{
	is_mesh_build_queued_ = false;
}
