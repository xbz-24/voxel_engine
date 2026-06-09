#include "Chunk.h"

#include "ChunkMeshBuilder.h"

#include <span>
#include <utility>

using ve::blocks::BlockRegistry;

void Chunk::BuildMesh(const BlockRegistry& blockRegistry, const ve::world::mesh::NeighborChunks& neighbors)
{
	UploadMesh(ve::world::mesh::BuildChunkMesh(*this, blockRegistry, neighbors));
}

void Chunk::UploadMesh(ve::world::mesh::ChunkMeshBuildResult meshBuildResult)
{
	_mesh.Upload(meshBuildResult.vertices, std::move(meshBuildResult.batches));
	_isMeshBuilt = true;
}

void Chunk::Draw(const BlockRegistry& blockRegistry, const ve::world::mesh::NeighborChunks& neighbors)
{
	if (NeedsMeshBuild())
	{
		BuildMesh(blockRegistry, neighbors);
	}
	_mesh.Draw();
}

ve::world::mesh::ChunkMeshInput Chunk::CreateMeshInput() const noexcept
{
	const auto* firstBlock = &blocks[0][0][0];
	return ve::world::mesh::ChunkMeshInput{
		_chunkX,
		_chunkZ,
		std::span<const ve::blocks::BlockId>(firstBlock, ve::world::mesh::ChunkBlockCount)
	};
}

bool Chunk::NeedsMeshBuild() const noexcept
{
	return !_isMeshBuilt;
}
