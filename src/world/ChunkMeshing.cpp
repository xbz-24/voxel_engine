#include "Chunk.h"

#include "ChunkMeshBuilder.h"

#include <span>
#include <utility>

using ve::blocks::BlockRegistry;

/// Builds a CPU mesh and uploads it immediately on the calling thread.
void Chunk::BuildMesh(const BlockRegistry& blockRegistry, const ve::world::mesh::NeighborChunks& neighbors)
{
	UploadMesh(ve::world::mesh::BuildChunkMesh(*this, blockRegistry, neighbors));
}

/// Uploads finished CPU mesh data to this chunk's GPU buffer.
void Chunk::UploadMesh(ve::world::mesh::ChunkMeshBuildResult meshBuildResult)
{
	_mesh.Upload(meshBuildResult.vertices, std::move(meshBuildResult.batches));
	_isMeshBuilt = true;
	_isMeshBuildQueued = false;
}

/// Returns the uploaded render mesh resource.
const ve::rendering::ChunkGpuMesh& Chunk::RenderMesh() const noexcept
{
	return _mesh;
}

/// Returns a read-only mesh input view over this chunk's block storage.
ve::world::mesh::ChunkMeshInput Chunk::CreateMeshInput() const noexcept
{
	const auto* firstBlock = &blocks[0][0][0];
	return ve::world::mesh::ChunkMeshInput{
		_chunkX,
		_chunkZ,
		std::span<const ve::blocks::BlockId>(firstBlock, ve::world::mesh::ChunkBlockCount)
	};
}

/// Reports whether this chunk needs a fresh mesh.
bool Chunk::NeedsMeshBuild() const noexcept
{
	return !_isMeshBuilt;
}

/**
 * Reserves this chunk for one async mesh task.
 *
 * @return True when the caller should enqueue a mesh build.
 */
bool Chunk::TryReserveMeshBuild() noexcept
{
	if (!NeedsMeshBuild() || _isMeshBuildQueued) return false;
	_isMeshBuildQueued = true;
	return true;
}

/// Releases a failed async mesh reservation without marking the current mesh valid.
void Chunk::CancelMeshBuildReservation() noexcept
{
	_isMeshBuildQueued = false;
}
