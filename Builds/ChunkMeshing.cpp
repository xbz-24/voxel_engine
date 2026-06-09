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
}

/// Draws the chunk, building its mesh lazily when needed.
void Chunk::Draw(const BlockRegistry& blockRegistry, const ve::world::mesh::NeighborChunks& neighbors)
{
	if (NeedsMeshBuild())
	{
		BuildMesh(blockRegistry, neighbors);
	}
	_mesh.Draw();
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
